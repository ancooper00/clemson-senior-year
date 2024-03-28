/* Database Systems, Coronel/Morris */
/* Type of SQL : MySQL */

DROP SCHEMA IF EXISTS Ch08_SALECO;
CREATE SCHEMA Ch08_SALECO;
use Ch08_SALECO;

CREATE TABLE CUSTOMER (
CUS_CODE int,
CUS_LNAME varchar(15),
CUS_FNAME varchar(15),
CUS_INITIAL varchar(1),
CUS_AREACODE varchar(3),
CUS_PHONE varchar(8),
CUS_BALANCE decimal(10,2)
);
INSERT INTO CUSTOMER VALUES('10010','Ramas','Alfred','A','615','844-2573','0');
INSERT INTO CUSTOMER VALUES('10011','Dunne','Leona','K','713','894-1238','0');
INSERT INTO CUSTOMER VALUES('10012','Smith','Kathy','W','615','894-2285','345.86');
INSERT INTO CUSTOMER VALUES('10013','Olowski','Paul','F','615','894-2180','536.75');
INSERT INTO CUSTOMER VALUES('10014','Orlando','Myron','','615','222-1672','0');
INSERT INTO CUSTOMER VALUES('10015','O''Brian','Amy','B','713','442-3381','0');
INSERT INTO CUSTOMER VALUES('10016','Brown','James','G','615','297-1228','221.19');
INSERT INTO CUSTOMER VALUES('10017','Williams','George','','615','290-2556','768.93');
INSERT INTO CUSTOMER VALUES('10018','Farriss','Anne','G','713','382-7185','216.55');
INSERT INTO CUSTOMER VALUES('10019','Smith','Olette','K','615','297-3809','0');


/* -- */


CREATE TABLE EMP (
EMP_NUM int,
EMP_TITLE varchar(4),
EMP_LNAME varchar(15),
EMP_FNAME varchar(15),
EMP_INITIAL varchar(1),
EMP_DOB datetime,
EMP_HIRE_DATE datetime,
EMP_AREACODE varchar(3),
EMP_PHONE varchar(8),
EMP_MGR int
);
INSERT INTO EMP VALUES('100','Mr.','Kolmycz','George','D','1942-06-15','1985-03-15','615','324-5456','');
INSERT INTO EMP VALUES('101','Ms.','Lewis','Rhonda','G','1965-03-19','1986-04-25','615','324-4472','100');
INSERT INTO EMP VALUES('102','Mr.','VanDam','Rhett','','1958-11-14','1990-12-20','901','675-8993','100');
INSERT INTO EMP VALUES('103','Ms.','Jones','Anne','M','1974-10-16','1994-08-28','615','898-3456','100');
INSERT INTO EMP VALUES('104','Mr.','Lange','John','P','1971-11-08','1994-10-20','901','504-4430','105');
INSERT INTO EMP VALUES('105','Mr.','Williams','Robert','D','1975-03-14','1998-11-08','615','890-3220','');
INSERT INTO EMP VALUES('106','Mrs.','Smith','Jeanine','K','1968-02-12','1989-01-05','615','324-7883','105');
INSERT INTO EMP VALUES('107','Mr.','Diante','Jorge','D','1974-08-21','1994-07-02','615','890-4567','105');
INSERT INTO EMP VALUES('108','Mr.','Wiesenbach','Paul','R','1966-02-14','1992-11-18','615','897-4358','');
INSERT INTO EMP VALUES('109','Mr.','Smith','George','K','1961-06-18','1989-04-14','901','504-3339','108');
INSERT INTO EMP VALUES('110','Mrs.','Genkazi','Leighla','W','1970-05-19','1990-12-01','901','569-0093','108');
INSERT INTO EMP VALUES('111','Mr.','Washington','Rupert','E','1966-01-03','1993-06-21','615','890-4925','105');
INSERT INTO EMP VALUES('112','Mr.','Johnson','Edward','E','1961-05-14','1983-12-01','615','898-4387','100');
INSERT INTO EMP VALUES('113','Ms.','Smythe','Melanie','P','1970-09-15','1999-05-11','615','324-9006','105');
INSERT INTO EMP VALUES('114','Ms.','Brandon','Marie','G','1956-11-02','1979-11-15','901','882-0845','108');
INSERT INTO EMP VALUES('115','Mrs.','Saranda','Hermine','R','1972-07-25','1993-04-23','615','324-5505','105');
INSERT INTO EMP VALUES('116','Mr.','Smith','George','A','1965-11-08','1988-12-10','615','890-2984','108');

/* -- */

CREATE TABLE EMPLOYEE (
EMP_NUM int,
EMP_TITLE varchar(4),
EMP_LNAME varchar(15),
EMP_FNAME varchar(15),
EMP_INITIAL varchar(1),
EMP_DOB datetime,
EMP_HIRE_DATE datetime,
EMP_AREACODE varchar(3),
EMP_PHONE varchar(8)
);
INSERT INTO EMP VALUES('100','Mr.','Kolmycz','George','D','1942-06-15','1985-03-15','615','324-5456','');
INSERT INTO EMP VALUES('101','Ms.','Lewis','Rhonda','G','1965-03-19','1986-04-25','615','324-4472','100');
INSERT INTO EMP VALUES('102','Mr.','VanDam','Rhett','','1958-11-14','1990-12-20','901','675-8993','100');
INSERT INTO EMP VALUES('103','Ms.','Jones','Anne','M','1974-10-16','1994-08-28','615','898-3456','100');
INSERT INTO EMP VALUES('104','Mr.','Lange','John','P','1971-11-08','1994-10-20','901','504-4430','105');
INSERT INTO EMP VALUES('105','Mr.','Williams','Robert','D','1975-03-14','1998-11-08','615','890-3220','');
INSERT INTO EMP VALUES('106','Mrs.','Smith','Jeanine','K','1968-02-12','1989-01-05','615','324-7883','105');
INSERT INTO EMP VALUES('107','Mr.','Diante','Jorge','D','1974-08-21','1994-07-02','615','890-4567','105');
INSERT INTO EMP VALUES('108','Mr.','Wiesenbach','Paul','R','1966-02-14','1992-11-18','615','897-4358','');
INSERT INTO EMP VALUES('109','Mr.','Smith','George','K','1961-06-18','1989-04-14','901','504-3339','108');
INSERT INTO EMP VALUES('110','Mrs.','Genkazi','Leighla','W','1970-05-19','1990-12-01','901','569-0093','108');
INSERT INTO EMP VALUES('111','Mr.','Washington','Rupert','E','1966-01-03','1993-06-21','615','890-4925','105');
INSERT INTO EMP VALUES('112','Mr.','Johnson','Edward','E','1961-05-14','1983-12-01','615','898-4387','100');
INSERT INTO EMP VALUES('113','Ms.','Smythe','Melanie','P','1970-09-15','1999-05-11','615','324-9006','105');
INSERT INTO EMP VALUES('114','Ms.','Brandon','Marie','G','1956-11-02','1979-11-15','901','882-0845','108');
INSERT INTO EMP VALUES('115','Mrs.','Saranda','Hermine','R','1972-07-25','1993-04-23','615','324-5505','105');
INSERT INTO EMP VALUES('116','Mr.','Smith','George','A','1965-11-08','1988-12-10','615','890-2984','108');

/* -- */

CREATE TABLE INVOICE (
INV_NUMBER int,
CUS_CODE int,
INV_DATE datetime
);
INSERT INTO INVOICE VALUES('1001','10014','2018-01-16');
INSERT INTO INVOICE VALUES('1002','10011','2018-01-16');
INSERT INTO INVOICE VALUES('1003','10012','2018-01-16');
INSERT INTO INVOICE VALUES('1004','10011','2018-01-17');
INSERT INTO INVOICE VALUES('1005','10018','2018-01-17');
INSERT INTO INVOICE VALUES('1006','10014','2018-01-17');
INSERT INTO INVOICE VALUES('1007','10015','2018-01-17');
INSERT INTO INVOICE VALUES('1008','10011','2018-01-17');

/* -- */

CREATE TABLE LINE (
INV_NUMBER int,
LINE_NUMBER int,
P_CODE varchar(10),
LINE_UNITS int,
LINE_PRICE decimal(8,2)
);
INSERT INTO LINE VALUES('1001','1','13-Q2/P2','1','14.99');
INSERT INTO LINE VALUES('1001','2','23109-HB','1','9.95');
INSERT INTO LINE VALUES('1002','1','54778-2T','2','4.99');
INSERT INTO LINE VALUES('1003','1','2238/QPD','1','38.95');
INSERT INTO LINE VALUES('1003','2','1546-QQ2','1','39.95');
INSERT INTO LINE VALUES('1003','3','13-Q2/P2','5','14.99');
INSERT INTO LINE VALUES('1004','1','54778-2T','3','4.99');
INSERT INTO LINE VALUES('1004','2','23109-HB','2','9.95');
INSERT INTO LINE VALUES('1005','1','PVC23DRT','12','5.87');
INSERT INTO LINE VALUES('1006','1','SM-18277','3','6.99');
INSERT INTO LINE VALUES('1006','2','2232/QTY','1','109.92');
INSERT INTO LINE VALUES('1006','3','23109-HB','1','9.95');
INSERT INTO LINE VALUES('1006','4','89-WRE-Q','1','256.99');
INSERT INTO LINE VALUES('1007','1','13-Q2/P2','2','14.99');
INSERT INTO LINE VALUES('1007','2','54778-2T','1','4.99');
INSERT INTO LINE VALUES('1008','1','PVC23DRT','5','5.87');
INSERT INTO LINE VALUES('1008','2','WR3/TT3','3','119.95');
INSERT INTO LINE VALUES('1008','3','23109-HB','1','9.95');

/* -- */

CREATE TABLE PRODUCT (
P_CODE varchar(10),
P_DESCRIPT varchar(35),
P_INDATE datetime,
P_QOH int,
P_MIN int,
P_PRICE decimal(8,2),
P_DISCOUNT decimal(3,2),
V_CODE int
);
INSERT INTO PRODUCT VALUES('11QER/31','Power painter, 15 psi., 3-nozzle','2017-11-03','8','5','109.99','0','25595');
INSERT INTO PRODUCT VALUES('13-Q2/P2','7.25-in. pwr. saw blade',         '2017-12-13','32','15', '14.99','0.05','21344');
INSERT INTO PRODUCT VALUES('14-Q1/L3','9.00-in. pwr. saw blade',         '2017-11-13','18','12','17.49','0','21344');
INSERT INTO PRODUCT VALUES('1546-QQ2','Hrd. cloth, 1/4-in., 2x50',       '2018-01-15','15','8','39.95','0','23119');
INSERT INTO PRODUCT VALUES('1558-QW1','Hrd. cloth, 1/2-in., 3x50',       '2018-01-15','23','5','43.99','0','23119');
INSERT INTO PRODUCT VALUES('2232/QTY','B\&D jigsaw, 12-in. blade',       '2017-12-30','8','5','109.92','0.05','24288');
INSERT INTO PRODUCT VALUES('2232/QWE','B\&D jigsaw, 8-in. blade',        '2017-12-24','6','5','99.87','0.05','24288');
INSERT INTO PRODUCT VALUES('2238/QPD','B\&D cordless drill, 1/2-in.',    '2018-01-20','12','5','38.95','0.05','25595');
INSERT INTO PRODUCT VALUES('23109-HB','Claw hammer',                     '2018-01-20','23','10','9.95','0.1','21225');
INSERT INTO PRODUCT VALUES('23114-AA','Sledge hammer, 12 lb.',           '2018-01-02','8','5','14.40','0.05','');
INSERT INTO PRODUCT VALUES('54778-2T','Rat-tail file, 1/8-in. fine',     '2017-12-15','43','20','4.99','0','21344');
INSERT INTO PRODUCT VALUES('89-WRE-Q','Hicut chain saw, 16 in.',         '2018-02-07','11','5','256.99','0.05','24288');
INSERT INTO PRODUCT VALUES('PVC23DRT','PVC pipe, 3.5-in., 8-ft',         '2018-02-20','188','75','5.87','0','');
INSERT INTO PRODUCT VALUES('SM-18277','1.25-in. metal screw, 25',        '2018-03-01','172','75','6.99','0','21225');
INSERT INTO PRODUCT VALUES('SW-23116','2.5-in. wd. screw, 50',           '2018-02-24','237','100','8.45','0','21231');
INSERT INTO PRODUCT VALUES('WR3/TT3','Steel matting, 4''x8''x1/6", .5" mesh','2018-01-17','18','5','119.95','0.1','25595');

/* -- */

CREATE TABLE VENDOR (
V_CODE int,
V_NAME varchar(15),
V_CONTACT varchar(50),
V_AREACODE varchar(3),
V_PHONE varchar(8),
V_STATE varchar(2),
V_ORDER varchar(1)
);
INSERT INTO VENDOR VALUES('21225','Bryson, Inc.','Smithson','615','223-3234','TN','Y');
INSERT INTO VENDOR VALUES('21226','SuperLoo, Inc.','Flushing','904','215-8995','FL','N');
INSERT INTO VENDOR VALUES('21231','D&E Supply','Singh','615','228-3245','TN','Y');
INSERT INTO VENDOR VALUES('21344','Gomez Bros.','Ortega','615','889-2546','KY','N');
INSERT INTO VENDOR VALUES('22567','Dome Supply','Smith','901','678-1419','GA','N');
INSERT INTO VENDOR VALUES('23119','Randsets Ltd.','Anderson','901','678-3998','GA','Y');
INSERT INTO VENDOR VALUES('24004','Brackman Bros.','Browning','615','228-1410','TN','N');
INSERT INTO VENDOR VALUES('24288','ORDVA, Inc.','Hakford','615','898-1234','TN','Y');
INSERT INTO VENDOR VALUES('25443','B&K, Inc.','Smith','904','227-0093','FL','N');
INSERT INTO VENDOR VALUES('25501','Damal Supplies','Smythe','615','890-3529','TN','N');
INSERT INTO VENDOR VALUES('25595','Rubicon Systems','Orton','904','456-0092','FL','Y');


