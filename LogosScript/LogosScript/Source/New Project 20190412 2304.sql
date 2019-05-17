-- MySQL Administrator dump 1.4
--
-- ------------------------------------------------------
-- Server version	5.6.37


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


--
-- Create schema agentstvo
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ agentstvo;
USE agentstvo;

--
-- Table structure for table `agentstvo`.`orders`
--

DROP TABLE IF EXISTS `orders`;
CREATE TABLE `orders` (
  `idOrders` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) DEFAULT NULL,
  `phone_number` varchar(11) DEFAULT NULL,
  `idRealty` int(11) DEFAULT NULL,
  PRIMARY KEY (`idOrders`),
  KEY `idRealty_idx` (`idRealty`),
  CONSTRAINT `idRealty` FOREIGN KEY (`idRealty`) REFERENCES `realty` (`idRealty`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `agentstvo`.`orders`
--

/*!40000 ALTER TABLE `orders` DISABLE KEYS */;
/*!40000 ALTER TABLE `orders` ENABLE KEYS */;


--
-- Table structure for table `agentstvo`.`realty`
--

DROP TABLE IF EXISTS `realty`;
CREATE TABLE `realty` (
  `idRealty` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) DEFAULT NULL,
  `number_of_rooms` int(2) DEFAULT NULL,
  `floor` int(3) DEFAULT NULL,
  `address` varchar(150) DEFAULT NULL,
  `image` varchar(256) DEFAULT NULL,
  `phone_number` decimal(20,0) DEFAULT NULL,
  `idUsers` int(11) DEFAULT NULL,
  `price` decimal(20,0) DEFAULT NULL,
  PRIMARY KEY (`idRealty`),
  KEY `idUsers_idx` (`idUsers`),
  CONSTRAINT `idUsers` FOREIGN KEY (`idUsers`) REFERENCES `users` (`idUsers`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `agentstvo`.`realty`
--

/*!40000 ALTER TABLE `realty` DISABLE KEYS */;
/*!40000 ALTER TABLE `realty` ENABLE KEYS */;


--
-- Table structure for table `agentstvo`.`users`
--

DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `idUsers` int(11) NOT NULL,
  `name` varchar(45) DEFAULT NULL,
  `surname` varchar(45) DEFAULT NULL,
  `phone_number` varchar(11) DEFAULT NULL,
  `login` varchar(45) DEFAULT NULL,
  `pass` varchar(32) DEFAULT NULL,
  `access` int(11) DEFAULT NULL,
  PRIMARY KEY (`idUsers`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `agentstvo`.`users`
--

/*!40000 ALTER TABLE `users` DISABLE KEYS */;
/*!40000 ALTER TABLE `users` ENABLE KEYS */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
