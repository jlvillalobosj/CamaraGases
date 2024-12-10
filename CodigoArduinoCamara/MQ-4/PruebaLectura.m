clear all;
clc;
% Configuración inicial
arduino_port = "COM7"; % Cambia 'COMx' por el puerto correcto, como 'COM3' o '/dev/ttyUSB0' en Linux
baud_rate = 9600; % Debe coincidir con el baud rate configurado en Arduino
sampling_time = 0.5; % Tiempo entre lecturas (debe coincidir con el delay en Arduino)

% Crear conexión con el puerto serie
s = serialport(arduino_port, baud_rate);

% Inicialización de variables
n_points = 1000; % Número máximo de puntos a graficar
ppm_values = zeros(1, n_points); % Array para almacenar valores
time_values = zeros(1, n_points); % Array para almacenar los tiempos
start_time = tic; % Inicia un cronómetro

% Configurar la figura para la gráfica
figure;
h = plot(time_values, ppm_values, 'LineWidth', 2);
xlabel('Tiempo (s)');
ylabel('Concentración CH4 (ppm)');
title('Concentración de CH4 en Tiempo Real');
grid on;
ylim([0, 20]); % Ajusta el rango según las mediciones esperadas

% Lectura y graficación en tiempo real
disp('Leyendo datos del Arduino...');
while true
    try
        % Leer un dato del puerto serie
        line = readline(s);
        ppm = str2double(line); % Convertir el dato a número
        disp(ppm);
        
        % Validar el dato
        if isnan(ppm)
            % warning('Dato no válido recibido, ignorando...');
            continue;
        end
        
        % Calcular el tiempo acumulado
        elapsed_time = toc(start_time); % Tiempo desde que comenzó la lectura
        
        % Desplazar los datos anteriores y añadir el nuevo
        ppm_values = [ppm_values(2:end), ppm];
        time_values = [time_values(2:end), elapsed_time]; % Actualizar el tiempo
        
        % Actualizar la gráfica
        set(h, 'YData', ppm_values, 'XData', time_values); % Actualizar tanto los valores de ppm como el tiempo
        drawnow;
        
        % Pausar para coincidir con la frecuencia de muestreo
        pause(sampling_time);
    catch ME
        warning('Error leyendo datos: %s', ME.message);
        break;
    end
end