/*
  Creado por Juan Manuel Hoyos y Cristhian Bergaño para el modulo Khaos I
  del observatorio sismologico de la Universidad del Quindío, este código
  recibe las tramas generadas por los modulos y guarda la información en una base
  de datos corriendo en el servidor MySQL mediante la implementación del conector
  MySQL/C++ referencias de :
  http://osdevlab.blogspot.com/2016/02/how-to-install-mysql-connectorc-on.html
  https://github.com/UDOOboard/serial_libraries_examples/blob/master/c/c_serial_example.c
*/
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <fstream>
#include <cstring>
#include <stdint.h>

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 90;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}

int main(void) {
    string s = "/dev/ttyUSB0";
    const char *portname = s.c_str();
    cout << portname;
    cout << '\n';
    int fd;
    int wlen;
    double time_spent = 0.0;
    uint32_t dataCh1[30];
    ofstream ch1 ("ch1.txt");
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error abriendo el puerto %s: %s\n", portname, strerror(errno));
        return -1;
    }
    set_interface_attribs(fd, B115200);    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    int cont = 0;
    int index = 0;
    uint8_t buf[91];
    int rdlen;

      do {
        cont++;
        rdlen = read(fd, buf, sizeof(buf));
        if (rdlen > 0) {
        #ifdef DISPLAY_STRING
        buf[rdlen] = 0;
        printf("Read %d: \"%s\"\n", rdlen, buf);
        #else /* display hex */
        unsigned char   *p;
        int i=0;
        printf("Read %d:", rdlen);
        for (p = buf; rdlen-- > 0; p++){
          printf(" 0x%x", *p);
        }
        printf("numero de paquete: %d \n", cont);
        printf("\n");
        #endif

        for (int k = 0; k < 30; k++ ){
          dataCh1[k] = 0;
          dataCh1[k]=(buf[index]<<16)|(buf[index+1]<<8)|(buf[index+2]);
          index+=3;
          printf("%c\n",dataCh1[k]); // Esta variable es la que se debe usar para enviar a la base de datos.
          if (ch1.is_open())
          {
            ch1 << dataCh1[k];
            ch1 << "\n";
          }
        }
        index = 0;
        cout << "Iniciando la conexión con la base de datos." << endl;
            try {
                sql::Driver *driver; /* Se crean los tres objetos del conector MySQL/C++*/
                sql::Connection *con;
                sql::Statement *stmt;

                /* Create a connection */
                driver = get_driver_instance();
                con = driver->connect("tcp://127.0.0.1:3306", "root", "9512"); //IP Address, user name, password

                stmt = con->createStatement();// Este crea las sentencias para ser ejecutadas.
                stmt->execute("DROP DATABASE IF EXISTS  test_db"); //drop if 'test_db' exists
                stmt->execute("CREATE DATABASE test_db");// create 'test_db' database

                stmt->execute("USE test_db"); //set current database as test_db
                stmt->execute("DROP TABLE IF EXISTS test"); //drop if 'test' table exists
                stmt->execute(
                        "CREATE TABLE test(id INT, label CHAR(1))"); //create table with (column name as id accepting INT) and (column name as label accepting CHAR(1))
                stmt->execute("INSERT INTO test(id, label) VALUES (1, 'a')"); //insert into 'test' table with (1 and 'a')

                delete stmt;
                delete con;
                /*According to documentation,
                You must free the sql::Statement and sql::Connection objects explicitly using delete
                But do not explicitly free driver, the connector object. Connector/C++ takes care of freeing that. */

            } catch (sql::SQLException &e) {
                cout << "# ERR: " << e.what();
                cout << " (MySQL error code: " << e.getErrorCode();
                cout << ", SQLState: " << e.getSQLState() << " )" << endl;
            }

        } else if (rdlen < 0) {
          printf("Error from read: %d: %s\n", rdlen, strerror(errno));
        } else {  /* rdlen == 0 */
          printf("Timeout from read\n");
        }
        /* repeat read to get full message */
      } while (cont < 2000);
    ch1.close();
    cout << "Successfully ended" << endl;
    return EXIT_SUCCESS;
}
