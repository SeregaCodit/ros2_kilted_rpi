#pragma once // гарантує відкриття файлу лише один раз при компіляції

#include <fcntl.h> // для open() відкриття файлів
#include <sys/ioctl.h> // ioctl() керування параметрами вводу-виводу
#include <linux/i2c-dev.h> // константи ля роботи з I2C драйвером ядра
#include <unistd.h> // для close(), read(), write() - базові операції з файлами
#include <cstdint> 
#include <stdexcept>



/**
 * I2CManager - обгортка над системними викликами Linux для шини I2C.
 * У Linux I2C-шина представлена як файл пристрою (наприклад, /dev/i2c-1).
 */

 class I2CManager
 {
public:
    /**
     * Конструктор - відкриває доступ до шини
     * @param device шлях до файлу (наприклад "/dev/i2c-1")
     * @param address I2C адреса датчика (наприклад 0x29)
     */
    I2CManager(const std::string& device, int address)
    {
        fd_= open(device.c_str(), O_RDWR); // відкриваємо файл пристрою в режимі читання-запису

        if (fd_ < 0){
            throw std::runtime_error("Could not open I2C bus: " + device);
        }

        // Налаштовуємо драйвер ядра на роботу з конкретною адресою пристрою (Slave)
        // всі операції з файлом fd_ відправляти на адресу address
        if (ioctl(fd_, I2C_SLAVE, address) < 0){  
            close(fd_); // закриваємо файл якщо налаштування не вдалось
            throw std::runtime_error("Could not set I2C address");
        }
    }
    
    
    ~I2CManager()
    {
        if (fd_ > 0) close(fd_);
    }


    /**
     * Записує один байт у вказаний регістр датчика.
     * @param reg номер регістра
     * @param value значення
     */
    void write_byte(uint8_t reg, uint8_t val){
        // I2C протокол запису: спочатку номер регістра, потім дані
        uint8_t buffer[2] = {reg, val};

        // записуємо 2 байти в чергу I2C
        if (write(fd_, buffer, 2) != 2){
            throw std::runtime_error("I2C write failed");
        }
    }


        /**
     * Читає 16-бітне число (word) з регістра.
     * Багато датчиків (як VL53L0X) повертають значення відстані двома байтами.
     */
    uint16_t read_word_data(uint8_t reg){

        // посилаємо запити що ми хочемо читати з файла fd_ все починаючи з регістра reg
        if (write(fd_, &reg, 1) < 0){
            throw std::runtime_error("I2C write before read failed");
        }

        // читаємо 2 байта відповіді в буфер
        uint8_t buffer[2];
        if (read(fd_, buffer, 2) != 2){
            throw std::runtime_error("I2C read failed on register");
        }

        /**
         * Перетворення Big Endian у Little Endian.
         * Датчик відправляє спочатку Старший байт (MSB), потім Молодший (LSB).
         * Процесор Raspberry Pi працює навпаки.
         * 
         * Операція: (байт_0 зсуваємо вліво на 8 біт) АБО (байт_1)
         * Приклад: buffer[0]=0x02, buffer[1]=0x58 (це 600 мм)
         * (0x02 << 8) = 0x0200
         * 0x0200 | 0x58 = 0x0258 (600 в десятковій)
         */
        return (static_cast<uint16_t>(buffer[0]) << 8) | buffer[1];
    }


    
    

private:
    int fd_; // File Descriptor - ідентифікатор відкритого файла в системі

 };