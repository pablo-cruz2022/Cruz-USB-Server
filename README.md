# Cruz USB Server

This is a simple and lightweight web server to run from a USB flash drive, SD card, CD-ROM, etc.. At least 1GB of storage is recommended to place the files into an external drive. The stack for this web
server is WEMP (Windows, NGINX, MariaDB, PHP). Yot can add an ```autorun.inf``` file into your drive to run the server automatically when the drive is inserted.

## Setup
Here are the steps to set up the portable web server.
- Download the source code zip file and extract it anywhere.
- Click on the .exe file and a console window will pop up.
- *DO NOT CLOSE THE CONSOLE POP-UP*.
- All the services will be live once the 'Press Ctrl-C to close' message appears.
- Enter <http://localhost> on your web browser. You should see a web page displaying the PHP info.
- Nginx and PHP are already configured. MariaDB needs further configuration to suit your needs.
- you can find each sub-directory for each service in the server/ folder.
- This program writes logs to the server/logs/ folder if you need to troubleshoot the server manager itself. Otherwise use the logs for each individual service in their respective log files.

### NGINX
The NGINX files and directories are in the ```nginx``` folder. It is structured so that it matches the Windows version, which can be downloaded [here](https://nginx.org/en/docs/windows.html).
- Edit the web pages in the ```html``` directory.
- There are three .html files in the above-stated directory which display error messages for the common http error codes. You can reconfigure this as you wish to yor needs.
- the index.php file is what is displaying the php info page. Edit this page to your needs.

### MariaDB
MariaDB will need further configuration to implement. These files can be found in the ```mariadb``` directory.
- You must go into the mariadb/bin directory and execute ```mariadb.exe -u root``` to further configure your database.
- The root user has no password, so you must create one and remember it.

### PHP
PHP does not need any further configuration and works right out of the box.


## Credits
This program is developed by Pablo Cruz as an open-source project. The following individuals have contributed to the development of this program:
* Pablo Cruz: Developer

This program is based on the work of other open-source projects, including Nginx, MariaDB, and PHP. The authors and contributors to these projects are listed below:

* Nginx: https://nginx.org/
* MariaDB: https://mariadb.com/
* PHP: https://www.php.net/

## License
This program is licensed under the [GNU Affero General Public License]([https://opensource.org/licenses/MIT](https://www.gnu.org/licenses/agpl-3.0.en.html)). The full license text is available in the `LICENSE` file included with this distribution.
