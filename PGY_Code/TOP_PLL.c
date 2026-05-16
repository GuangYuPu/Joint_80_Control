#include "TOP_PLL.h"

Top_Pll_t Top_Rotor_Hall, Top_Axis_Hall;

void TOP_PLL_Init(Top_Pll_t *Top_Hall)
{
    Top_Hall->kp      = 200000;
    Top_Hall->ki      = 300000;
    Top_Hall->kd      = 1000;
    Top_Hall->limit   = 1e6;
    Top_Hall->limitwe = TPI_PGY;

    Top_Hall->error[0]   = 0;
    Top_Hall->error[1]   = 0;
    Top_Hall->accelarate = 0;
    Top_Hall->we         = 0;
    Top_Hall->theta_e    = 0;
    Top_Hall->amplitude  = 1;

    Top_Hall->n_e           = 0;
    Top_Hall->n_m           = 0;
    Top_Hall->TOP_PLL_Theta = 0;
}

void TOP_PLL_Loop(Top_Pll_t *Top_Hall, float alfa, float beta, float Ts, float np)
{
    float Amplitude = 0;
    float d_theta   = 0;

    Amplitude = sqrtf(alfa * alfa + beta * beta);
    alfa      = alfa / Amplitude;
    beta      = beta / Amplitude;

    Top_Hall->error[0] = alfa * cosf(Top_Hall->theta_e) - beta * sinf(Top_Hall->theta_e);
    Top_Hall->accelarate += Top_Hall->kp * (Top_Hall->error[0] - Top_Hall->error[1]) + Top_Hall->ki * Ts * Top_Hall->error[0];
    Top_Hall->accelarate = _constrain(Top_Hall->accelarate, -Top_Hall->limit, Top_Hall->limit);

    Top_Hall->we += Ts * Top_Hall->accelarate;
    d_theta = Top_Hall->we + Top_Hall->kd * Top_Hall->error[0];
    d_theta = _constrain(d_theta, -Top_Hall->limitwe / Ts, Top_Hall->limitwe / Ts);

    Top_Hall->theta_e += Ts * d_theta;
    if (Top_Hall->theta_e > TPI_PGY) Top_Hall->theta_e -= TPI_PGY;
    if (Top_Hall->theta_e < 0) Top_Hall->theta_e += TPI_PGY;

    Top_Hall->TOP_PLL_Theta = (int16_t)((uint16_t)(Top_Hall->theta_e / TPI_PGY * 65535) - OrthATOCALIB.EroMeanRec);

    Top_Hall->n_e = Top_Hall->we / TPI_PGY;
    Top_Hall->n_m = Top_Hall->n_e / np;

    Top_Hall->error[1] = Top_Hall->error[0];
}

void NOTOP_PLL_Loop(Top_Pll_t *Top_Hall, float alfa, float beta, float Ts, float np, float delta)
{
    static uint16_t num     = 0;
    float Amplitude         = 1000;
    float d_theta           = 0;

    alfa = alfa / (Top_Hall->amplitude * Amplitude);
    beta = beta / (Top_Hall->amplitude * Amplitude);

    Top_Hall->error[0] = alfa * cosf(Top_Hall->theta_e - delta) - beta * sinf(Top_Hall->theta_e);
    Top_Hall->accelarate += Top_Hall->kp * (Top_Hall->error[0] - Top_Hall->error[1]) + Top_Hall->ki * Ts * Top_Hall->error[0];
    Top_Hall->accelarate = _constrain(Top_Hall->accelarate, -Top_Hall->limit, Top_Hall->limit);

    Top_Hall->we += Ts * Top_Hall->accelarate;
    d_theta = Top_Hall->we + Top_Hall->kd * Top_Hall->error[0];
    d_theta = _constrain(d_theta, -Top_Hall->limitwe / Ts, Top_Hall->limitwe / Ts);

    Top_Hall->theta_e += Ts * d_theta;
    if (Top_Hall->theta_e > TPI_PGY) Top_Hall->theta_e -= TPI_PGY;
    if (Top_Hall->theta_e < 0) Top_Hall->theta_e += TPI_PGY;

    Top_Hall->TOP_PLL_Theta = (int16_t)((uint16_t)(Top_Hall->theta_e / TPI_PGY * 65535) - OrthATOCALIB.EroMeanRec);

    Top_Hall->n_e = Top_Hall->we / TPI_PGY;
    Top_Hall->n_m = Top_Hall->n_e / np;

    Top_Hall->error[1] = Top_Hall->error[0];

    if ((Top_Hall->error[1] * Top_Hall->error[1]) < 1e-6 && (Top_Hall->error[0] * Top_Hall->error[0]) < 1e-6) {
        num++;
        if (num >= 10) {
            Top_Hall->amplitude = 0.8 * Top_Hall->amplitude + 0.2 * (alfa * sinf(Top_Hall->theta_e - delta) + beta * cosf(Top_Hall->theta_e));
            num                 = 0;
        }
    }
    else{
        num = 0;
    }
}

/*----------compensation-----------*/

int16_t comp_I[COMP_TABLE_SIZE] = {0};
uint8_t calib_valid[COMP_TABLE_SIZE] = {0};
uint8_t calib_done                   = 1;

void Axis_Pos_Comp_Loop(void)
{
    static uint8_t align_flag = 0;
    theta_f += (Top_Rotor_Hall.we / (35 * 10)) * 1e-3;
    if (Top_Axis_Hall.TOP_PLL_Theta <= -32700 || Top_Axis_Hall.TOP_PLL_Theta >= 32700) {
        theta_f    = ((float)Top_Axis_Hall.TOP_PLL_Theta) * TPI_PGY / 65535;
        align_flag = 1;
    }
    theta_I = (int16_t)(theta_f / TPI_PGY * 65535);

    if (align_flag) {
        // 标定阶段
        if (!calib_done) {
            // 计算当前theta_I对应的索引 (0~359)
            int32_t idx32 = ((int32_t)theta_I - INT16_MIN) * COMP_TABLE_SIZE / RANGE;
            uint16_t idx  = (uint16_t)idx32;
            if (idx >= COMP_TABLE_SIZE) idx = COMP_TABLE_SIZE - 1;

            // 当theta_I处于该区间中心附近±30范围内时记录补偿值
            int32_t center = INT16_MIN + (idx32 * RANGE / COMP_TABLE_SIZE) + (RANGE / (2 * COMP_TABLE_SIZE));
            if (theta_I >= center - 30 && theta_I <= center + 30) {
                comp_I[idx]      = theta_I - Top_Axis_Hall.TOP_PLL_Theta;
                calib_valid[idx] = 1;
            }

            // 检查是否所有区间都已标定
            uint8_t all_done = 1;
            for (uint16_t i = 0; i < COMP_TABLE_SIZE; i++) {
                if (!calib_valid[i]) {
                    all_done = 0;
                    break;
                }
            }
            if (all_done) {
                calib_done = 1;
            }
            return; // 标定阶段不输出补偿
        }

        // 补偿阶段
        if (calib_done) {
            // 计算索引及区间边界
            int32_t idx32 = ((int32_t)theta_I - INT16_MIN) * COMP_TABLE_SIZE / RANGE;
            uint16_t idx  = (uint16_t)idx32;
            if (idx >= COMP_TABLE_SIZE) idx = COMP_TABLE_SIZE - 1;

            uint16_t idx_next = (idx == COMP_TABLE_SIZE - 1) ? 0 : idx + 1;

            int32_t lower = INT16_MIN + (idx32 * RANGE / COMP_TABLE_SIZE);
            int32_t upper = INT16_MIN + ((idx32 + 1) * RANGE / COMP_TABLE_SIZE);

            // 线性插值计算补偿值
            int32_t delta_comp = (int32_t)comp_I[idx_next] - comp_I[idx];
            int32_t range      = upper - lower;
            int32_t offset     = (int32_t)theta_I - lower;

            int32_t comp_val = comp_I[idx];
            if (range != 0) {
                comp_val += (delta_comp * offset) / range;
            }

            // 输出补偿后的位置
            Theta_O = Top_Axis_Hall.TOP_PLL_Theta + (int16_t)comp_val;
        }
    }
}
