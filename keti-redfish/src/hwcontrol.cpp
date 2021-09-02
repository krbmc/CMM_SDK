#include "hwcontrol.hpp"

int mem_fd;
void *gpio_map;
volatile unsigned *gpio;

bool init_gpio(void)
{
	/* open /dev/mem */
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
	{
		log(error) << "Cannot open /dev/mem";
		return false;
	}

	/* mmap GPIO */
	gpio_map = mmap(
		NULL,					//Any adddress in our space will do
		BLOCK_SIZE,				//Map length
		PROT_READ | PROT_WRITE, // Enable reading & writting to mapped memory
		MAP_SHARED,				//Shared with other processes
		mem_fd,					//File to map
		GPIO_BASE				//Offset to GPIO peripheral
	);

	close(mem_fd); //No need to keep mem_fd open after mmap

	if (gpio_map == MAP_FAILED)
	{
		log(error) << "mmap error " << (int)gpio_map; //errno also set!
		return false;
	}

	// Always use volatile pointer!
	gpio = (volatile unsigned *)gpio_map;

	// Set LED IN/OUT mode
	INP_GPIO(LED_RED); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(LED_RED);
	INP_GPIO(LED_YELLOW); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(LED_YELLOW);
	INP_GPIO(LED_GREEN); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(LED_GREEN);

	return true;

} // setup_io

bool init_i2c(void)
{
	int file;

	if ((file = open(DEV_I2C, O_RDWR)) < 0)
	{
		log(error) << "Failed to detect " << DEV_I2C;
		return false;
	}
	else if (ioctl(file, I2C_SLAVE, SLAVE) < 0)
	{
		stringstream stream;
		stream << hex << SLAVE;
		log(error) << "Failed to detect bus 0x" << string(stream.str());
		return false;
	}
	close(file);

	return true;
}

double get_intake_humidity(uint8_t _sensor_index)
{
	int file;
	float value;
	char cmd[2];
	char buf[8];

	// Open I2C device file
	if ((file = open(DEV_I2C, O_RDWR)) < 0)
	{
		log(error) << "Cannot open device file";
		close(file);
		return -1;
	}

	if (ioctl(file, I2C_SLAVE, SLAVE) < 0)
	{
		stringstream stream;
		stream << hex << SLAVE;
		log(error) << "Failed to detect bus 0x" << string(stream.str());
		close(file);
		return -1;
	}

	// TODO 센서 번호 범위 수정
	if (_sensor_index < 0 || _sensor_index > 3)
	{
		log(error) << "Out of sensor index number";
		close(file);
		return -1;
	}

	// Set I2C request command
	cmd[0] = (char)_sensor_index;
	cmd[1] = 2;

	if (write(file, cmd, 2) < 0)
	{
		log(error) << "Cannot write device file";
		close(file);
		return -1;
	}
	usleep(10000); // 10000
	read(file, buf, 8);
	value = atof(buf);
	close(file);

	return (double)value;
}

bool get_intake_temperature_config(double *_range)
{
	int file;
	char cmd[2];
	char buf[16];

	// Open I2C device file
	if ((file = open(DEV_I2C, O_RDWR)) < 0)
	{
		log(error) << "Cannot open device file";
		close(file);
		return false;
	}

	if (ioctl(file, I2C_SLAVE, SLAVE) < 0)
	{
		stringstream stream;
		stream << hex << SLAVE;
		log(error) << "Failed to detect bus 0x" << string(stream.str());
		close(file);
		return false;
	}

	// Set I2C request command
	cmd[0] = 1;
	cmd[1] = 3;

	if (write(file, cmd, 2) < 0)
	{
		log(error) << "Cannot write device file";
		close(file);
		return false;
	}

	usleep(10000); // 10000
	read(file, buf, 16);
	sscanf(buf, "%lf,%lf", &_range[0], &_range[1]);
	close(file);

	return true;
}

double get_intake_temperature(uint8_t _sensor_index)
{
	int file;
	double value;
	char cmd[2];
	char buf[8];

	// Open I2C device file
	if ((file = open(DEV_I2C, O_RDWR)) < 0)
	{
		log(error) << "Cannot open device file";
		close(file);
		return -1;
	}

	if (ioctl(file, I2C_SLAVE, SLAVE) < 0)
	{
		stringstream stream;
		stream << hex << SLAVE;
		log(error) << "Failed to detect bus 0x" << string(stream.str());
		close(file);
		return -1;
	}

	// TODO 센서 번호 범위 수정
	if (_sensor_index < 0 || _sensor_index > 3)
	{
		log(error) << "Out of sensor index number";
		close(file);
		return -1;
	}

	// Set I2C request command
	cmd[0] = (char)_sensor_index;
	cmd[1] = 4;

	if (write(file, cmd, 2) < 0)
	{
		log(error) << "Cannot write device file";
		close(file);
		return -1;
	}
	usleep(10000); // 10000
	read(file, buf, 8);
	value = atof(buf);
	close(file);

	return value;
}

double get_cpu_temperature(uint8_t _sensor_index)
{
	double value;
	FILE *file = NULL;
	char buf[16] = {0};

	if ((file = fopen("/sys/class/thermal/thermal_zone0/temp", "r")) == NULL)
	{
		fclose(file);
		return -1;
	}

	if (fgets(buf, sizeof(buf), file) == NULL)
	{
		fclose(file);
		return -1;
	}

	buf[strlen(buf) - 1] = '\0';
	value = atof(buf) / 1000.0;
	fclose(file);
	
	return value;
}