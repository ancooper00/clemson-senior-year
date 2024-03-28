/*
    Zach Clay and Ashlyn Cooper
*/

CREATE SCHEMA IF NOT EXISTS PizzasRUs;

USE PizzasRUs;

CREATE TABLE basePriceCost (
  BasePriceCostCrustType VARCHAR(20) NOT NULL,
  BasePriceCostPizzaSize VARCHAR(15) NOT NULL,
  BasePriceCostCost DECIMAL(5,2) NOT NULL,
  BasePriceCostPrice DECIMAL(5,2) NOT NULL,
  PRIMARY KEY (BasePriceCostCrustType, BasePriceCostPizzaSize),
  FULLTEXT (BasePriceCostPizzaSize)
);

CREATE TABLE customer (
  CustomerID INT NOT NULL AUTO_INCREMENT,
  CustomerName VARCHAR(100) NOT NULL,
  CustomerPhone VARCHAR(20),
  PRIMARY KEY (CustomerID)
);

CREATE TABLE pizzaOrder (
  PizzaOrderID INT NOT NULL AUTO_INCREMENT,
  PizzaOrderCustomerID INT,
  PizzaOrderType VARCHAR(15) NOT NULL,
  PizzaOrderCost DECIMAL(6,2) NOT NULL,
  PizzaOrderPrice DECIMAL(6,2) NOT NULL,
  PizzaOrderTime TIMESTAMP NOT NULL,
  PRIMARY KEY (PizzaOrderID),
  FOREIGN KEY (PizzaOrderCustomerID) REFERENCES customer(CustomerID)
);

CREATE TABLE dineIn (
  DineInOrderID INT NOT NULL,
  DineInTableNum INT NOT NULL,
  PRIMARY KEY (DineInOrderID),
  FOREIGN KEY (DineInOrderID) REFERENCES pizzaOrder(PizzaOrderID)
);

CREATE TABLE pickup (
  PickupOrderID INT NOT NULL,
  PRIMARY KEY (PickupOrderID),
  FOREIGN KEY (PickupOrderID) REFERENCES pizzaOrder(PizzaOrderID)
);

CREATE TABLE delivery (
  DeliveryOrderID INT NOT NULL,
  DeliveryAddress VARCHAR(200) NOT NULL,
  PRIMARY KEY (DeliveryOrderID),
  FOREIGN KEY (DeliveryOrderID) REFERENCES pizzaOrder(PizzaOrderID)
);

CREATE TABLE pizza (
  PizzaID INT NOT NULL AUTO_INCREMENT,
  PizzaOrderID INT NOT NULL,
  PizzaBaseCrustType VARCHAR(20) NOT NULL,
  PizzaBasePizzaSize VARCHAR(15) NOT NULL,
  PizzaPrice DECIMAL(6,2) NOT NULL,
  PizzaCost DECIMAL(6,2) NOT NULL,
  PizzaStatus TINYINT NOT NULL,
  PRIMARY KEY (PizzaID),
  FOREIGN KEY (PizzaOrderID) REFERENCES pizzaOrder(PizzaOrderID),
  FOREIGN KEY (PizzaBaseCrustType, PizzaBasePizzaSize) REFERENCES basePriceCost(BasePriceCostCrustType, BasePriceCostPizzaSize)
);

CREATE TABLE topping (
  ToppingName VARCHAR(50) NOT NULL,
  ToppingPrice DECIMAL(5,2) NOT NULL,
  ToppingCost DECIMAL(5,2) NOT NULL,
  ToppingAmtSmall DECIMAL(4,2) NOT NULL,
  ToppingAmtMedium DECIMAL(4,2) NOT NULL,
  ToppingAmtLarge DECIMAL(4,2) NOT NULL,
  ToppingAmtXLarge DECIMAL(4,2) NOT NULL,
  ToppingMinInventory INT,
  ToppingCurrInventory INT NOT NULL,
  PRIMARY KEY (ToppingName)
);

CREATE TABLE pizzaTopping (
  PizzaToppingPizzaID INT NOT NULL,
  PizzaToppingName VARCHAR(50) NOT NULL,
  PizzaToppingQuantity INT NOT NULL,
  PRIMARY KEY (PizzaToppingPizzaID, PizzaToppingName),
  FOREIGN KEY (PizzaToppingPizzaID) REFERENCES pizza(PizzaID),
  FOREIGN KEY (PizzaToppingName) REFERENCES topping(ToppingName)
);

CREATE TABLE discount (
  DiscountID INT NOT NULL AUTO_INCREMENT,
  DiscountName VARCHAR(50) NOT NULL,
  DiscountAmount DECIMAL(6,2) NOT NULL,
  DiscountIsPercent TINYINT NOT NULL,
  PRIMARY KEY (DiscountID)
);

CREATE TABLE discountToPizza (
  DiscountToPizzaDiscountID INT NOT NULL,
  DiscountToPizzaPizzaID INT NOT NULL,
  PRIMARY KEY (DiscountToPizzaDiscountID, DiscountToPizzaPizzaID),
  FOREIGN KEY (DiscountToPizzaDiscountID) REFERENCES discount(DiscountID),
  FOREIGN KEY (DiscountToPizzaPizzaID) REFERENCES pizza(PizzaID)
);

CREATE TABLE discountToOrder (
  DiscountToOrderID INT NOT NULL,
  DiscountToOrderOrderID INT NOT NULL,
  PRIMARY KEY (DiscountToOrderID, DiscountToOrderOrderID),
  FOREIGN KEY (DiscountToOrderID) REFERENCES discount(DiscountID),
  FOREIGN KEY (DiscountToOrderOrderID) REFERENCES pizzaOrder(PizzaOrderID)
);