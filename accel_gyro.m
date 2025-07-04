clear;

FILENAME = "accel_gyro_printf.csv";
IDLE_SAMPLES = 0;
DT = 0.01;
ALPHA = 0.9;
#accel_cal = [-0.100216722972972, 1.35917060472973, 0.0153553790169492];
#gyro_cal = [0.001169915805, -0.056912726, 0.0150643665];
accel_cal_offsets = [0, 0, 0];
accel_cal_gains = [1, 1, 1];
gyro_cal_offsets = [0, 0, 0];
gyro_cal_gains = [1, 1, 1];


function [res_offset_x, res_offset_y, res_offset_z, res_gain_x, res_gain_y, res_gain_z] = calc_cal_accel(accel_data_x, accel_data_y, accel_data_z, drop_samples, mean_samples)

  DROP_SAMPLES = drop_samples;
  MEAN_SAMPLES = mean_samples;

  sorted_data_x = sort(accel_data_x);
  sorted_data_y = sort(accel_data_y);
  sorted_data_z = sort(accel_data_z);

  min_x = mean(sorted_data_x(1:MEAN_SAMPLES));
  max_x = mean(sorted_data_x(end-MEAN_SAMPLES+1:end));

  min_y = mean(sorted_data_y(1:MEAN_SAMPLES));
  max_y = mean(sorted_data_y(end-MEAN_SAMPLES+1:end));

  min_z = mean(sorted_data_z(1:MEAN_SAMPLES));
  max_z = mean(sorted_data_z(end-MEAN_SAMPLES+1:end));

  offset_x = (max_x + min_x) / 2;
  offset_y = (max_y + min_y) / 2;
  offset_z = (max_z + min_z) / 2;

  gain_x = 2.0 / (max_x - min_x);
  gain_y = 2.0 / (max_y - min_y);
  gain_z = 2.0 / (max_z - min_z);

  res_offset_x = offset_x;
  res_offset_y = offset_y;
  res_offset_z = offset_z;

  res_gain_x = gain_x;
  res_gain_y = gain_y;
  res_gain_z = gain_z;

endfunction


function [res_offset_x, res_offset_y, res_offset_z, res_gain_x, res_gain_y, res_gain_z] = calc_cal_gyro(gyro_data_x, gyro_data_y, gyro_data_z, drop_samples, mean_samples)

  DROP_SAMPLES = drop_samples;
  MEAN_SAMPLES = mean_samples;

  sorted_data_x = sort(gyro_data_x(DROP_SAMPLES:end-DROP_SAMPLES));
  sorted_data_y = sort(gyro_data_y(DROP_SAMPLES:end-DROP_SAMPLES));
  sorted_data_z = sort(gyro_data_z(DROP_SAMPLES:end-DROP_SAMPLES));

  from = floor((length(sorted_data_x) - MEAN_SAMPLES) / 2);
  to = from + MEAN_SAMPLES;
  offset_x = mean(sorted_data_x(from:to));
  offset_y = mean(sorted_data_y(from:to));
  offset_z = mean(sorted_data_z(from:to));

  gain_x = 1.0;
  gain_y = 1.0;
  gain_z = 1.0;

##  x = [1:length(sorted_data_x)];
##  plot(x, sorted_data_x, x, sorted_data_y, x, sorted_data_z);
##  legend("x", "y", "z");

  res_offset_x = offset_x;
  res_offset_y = offset_y;
  res_offset_z = offset_z;

  res_gain_x = gain_x;
  res_gain_y = gain_y;
  res_gain_z = gain_z;

endfunction


function res_data = calibrate(data, offsets, gains)
  data_size = size(data);
  rows = data_size(1);
  cols = data_size(2);

  res_data = zeros(rows, cols);

  for i=[1:rows]
    res_data(i, 1) = (data(i, 1) - offsets(1)) * gains(1);
    res_data(i, 2) = (data(i, 2) - offsets(2)) * gains(2);
    res_data(i, 3) = (data(i, 3) - offsets(3)) * gains(3);
  endfor

endfunction


function [res_roll, res_pitch] = calc_accel_roll_pitch(data)
  data_size = size(data);
  rows = data_size(1);
  #cols = data_size(2);

  res_roll = zeros(rows, 1);
  res_pitch = zeros(rows, 1);

  acc_x = 0;
  acc_y = 0;
  acc_z = 0;

  roll = 0;
  pitch = 0;

  for i=[1:rows]
    acc_x = data(i, 1);
    acc_y = data(i, 2);
    acc_z = data(i, 3);

    roll = atan2(-acc_x, acc_y);
    pitch = atan2(-acc_x, -acc_z);

    res_roll(i) = roll;
    res_pitch(i) = pitch;
  endfor
endfunction


function [res_droll, res_dpitch] = calc_gyro_droll_dpitch(dt, data)
  data_size = size(data);
  rows = data_size(1);
  #cols = data_size(2);

  res_droll = zeros(rows, 1);
  res_dpitch = zeros(rows, 1);

  gyro_x = 0;
  gyro_y = 0;
  gyro_z = 0;

  droll = 0;
  dpitch = 0;

  for i=[1:rows]
    gyro_x = data(i, 1);
    gyro_y = data(i, 2);
    gyro_z = data(i, 3);

    droll = -gyro_z * dt;
    dpitch = -gyro_y * dt;

    res_droll(i) = droll;
    res_dpitch(i) = dpitch;
  endfor
endfunction


function [res_roll, res_pitch] = calc_roll_pitch(accel_roll, accel_pitch, gyro_droll, gyro_dpitch, alpha)
  data_size = size(accel_roll);
  rows = data_size(1);
  #cols = data_size(2);

  res_roll = zeros(rows, 1);
  res_pitch = zeros(rows, 1);

  acc_roll = 0;
  acc_pitch = 0;

  gy_droll = 0;
  gy_dpitch = 0;

  gy_roll = 0;
  gy_pitch = 0;

  roll = 0;
  pitch = 0;

  for i=[1:rows]
    acc_roll = accel_roll(i);
    acc_pitch = accel_pitch(i);

    gy_droll = gyro_droll(i);
    gy_dpitch = gyro_dpitch(i);

    gy_roll = roll + gy_droll;
    gy_pitch = pitch + gy_dpitch;

    roll = gy_roll * alpha + (1.0 - alpha) * acc_roll;
    pitch = gy_pitch * alpha + (1.0 - alpha) * acc_pitch;

    res_roll(i) = roll;
    res_pitch(i) = pitch;
  endfor
endfunction


# https://gist.github.com/zasimov/25ba4ca78a3bfe82cedd77b1795d8de8
# https://habr.com/ru/articles/503542/
# https://habr.com/ru/companies/singularis/articles/516798/
# https://habr.com/ru/articles/166693/
# https://habr.com/ru/articles/140274/

function res_kf = kalman_init(X0, P0, F, B, Q, H, R)
  kf = struct(
              "X", X0,
              "Xdash", [0; 0],
              "P", P0,
              "Pdash", [0; 0],
              "F", F,
              "FT", transpose(F),
              "B", B,
              "Q", Q,
              "H", H,
              "HT", transpose(H),
              "R", R,
              "I", eye(2),
              "_", 0
              );
  #
  res_kf = kf;
endfunction

function res_kf = kalman_predict(kf, u, dt)

  kf.Xdash = kf.F * kf.X + kf.B * u * dt;

  #kf.Pdash = kf.F * kf.P * kf.FT + kf.Q;
  kf.Pdash = kf.F * kf.P * kf.FT + kf.Q;

  res_kf = kf;
endfunction

function res_kf = kalman_update(kf, z)

  S = kf.H * kf.Pdash * kf.HT + kf.R;
  invS = inverse(S);

  K = kf.Pdash * kf.HT * invS;

  y = z - kf.H * kf.Xdash;

  kf.X = kf.Xdash + K * y;
  kf.P = (kf.I - K * kf.H) * kf.Pdash;

  res_kf = kf;
endfunction


function res_kf = kalman_calc(kf, u, dt, z)
  kf = kalman_predict(kf, u, dt);
  kf = kalman_update(kf, z);

  res_kf = kf;
endfunction


#data = csvread("accel_gyro_data.csv");
data = csvread(FILENAME);

SAMPLES = length(data);
t = linspace(0, DT * (SAMPLES-1), SAMPLES);
x = [1:1:SAMPLES];


accel_data = data(:, 1:3);
gyro_data = data(:, 4:6);

ACCEL_CAL_DATA_LEN = SAMPLES;
acx = x(1:ACCEL_CAL_DATA_LEN);

accel_data_x = accel_data(1:ACCEL_CAL_DATA_LEN, 1);
accel_data_y = accel_data(1:ACCEL_CAL_DATA_LEN, 2);
accel_data_z = accel_data(1:ACCEL_CAL_DATA_LEN, 3);

[aox, aoy, aoz, agx, agy, agz] = calc_cal_accel(accel_data_x, accel_data_y, accel_data_z, 100, 200)


GYRO_CAL_DATA_LEN = SAMPLES;
gcx = x(1:GYRO_CAL_DATA_LEN);

gyro_data_x = gyro_data(1:GYRO_CAL_DATA_LEN, 1);
gyro_data_y = gyro_data(1:GYRO_CAL_DATA_LEN, 2);
gyro_data_z = gyro_data(1:GYRO_CAL_DATA_LEN, 3);

[gox, goy, goz, ggx, ggy, ggz] = calc_cal_gyro(gyro_data_x, gyro_data_y, gyro_data_z, 100, 200)


accel_cal_offsets = [aox, aoy, aoz];
accel_cal_gains = [agx, agy, agz];
gyro_cal_offsets = [gox, goy, goz];
gyro_cal_gains = [ggx, ggy, ggz];


##plot(cx, accel_cal_data(:, 1),
##     cx, accel_cal_data(:, 2),
##     cx, accel_cal_data(:, 3))
##plot(acx, accel_data_x,
##     acx, accel_data_y,
##     acx, accel_data_z);
##plot(gcx, gyro_data_x,
##     gcx, gyro_data_y,
##     gcx, gyro_data_z);
###
##legend("x", "y", "z");


accel_data_cal = calibrate(accel_data, accel_cal_offsets, accel_cal_gains);
gyro_data_cal = calibrate(gyro_data, gyro_cal_offsets, gyro_cal_gains);
#gyro_data_cal = gyro_data;

#
##plot(acx, accel_data_cal(:, 1),
##     acx, accel_data_cal(:, 2),
##     acx, accel_data_cal(:, 3));
##plot(gcx, gyro_data_cal(:, 1),
##     gcx, gyro_data_cal(:, 2),
##     gcx, gyro_data_cal(:, 3));
###
##legend("x", "y", "z");
#

[accel_roll, accel_pitch] = calc_accel_roll_pitch(accel_data_cal);
[gyro_droll, gyro_dpitch] = calc_gyro_droll_dpitch(DT, gyro_data_cal);

[roll, pitch] = calc_roll_pitch(accel_roll, accel_pitch, gyro_droll, gyro_dpitch, ALPHA);

#kalman_init(X0, P0, F, B, Q, H, R)
X0 = [0; 0];
P0 = [0, 0;
      0, 0];
F = [1, 0;
     0, 0];
B = [0; 1];
Q = [0; 0];
H = [1 0];
R = [0];
kf_roll = kalman_init(X0, P0, F, B, Q, H, R);
kf_pitch = kalman_init(X0, P0, F, B, Q, H, R);

roll_kf = zeros(SAMPLES, 1);
pitch_kf = zeros(SAMPLES, 1);

for i=[1:SAMPLES]
  kf_roll = kalman_calc(kf_roll, gyro_droll(i), 1.0, accel_roll(i));
  roll_kf(i) = kf_roll.X(1);
endfor

#
plot(t, accel_roll, t, accel_pitch, t, roll, t, pitch);
grid on;
grid minor on;

