/*
    Zach Clay and Ashlyn Cooper
*/

USE PizzasRUs;

INSERT INTO topping 
  (ToppingName, ToppingPrice, ToppingCost, ToppingAmtSmall, ToppingAmtMedium, ToppingAmtLarge, ToppingAmtXLarge, ToppingCurrInventory) 
VALUES 
  ('Pepperoni',         1.25, 0.2,   2,   2.75, 3.5,  4.5,  100),
  ('Sausage',           1.25, 0.15,  2.5, 3,    3.5,  4.25, 100),
  ('Ham',               1.5,  0.15,  2,   2.5,  3.25, 4,    78),
  ('Chicken',           1.75, 0.25,  1.5, 2,    2.25, 3,    56),
  ('Green Pepper',      0.5,  0.02,  1,   1.5,  2,    2.5,  79),
  ('Onion',             0.5,  0.02,  1,   1.5,  2,    2.75, 85),
  ('Roma Tomato',       0.75, 0.03,  2,   3,    3.5,  4.5,  86),
  ('Mushrooms',         0.75, 0.1,   1.5, 2,    2.5,  3,    52),
  ('Black Olives',      0.6,  0.1,  0.75, 1,    1.5,  2,    39),
  ('Pineapple',         1,    0.25,  1,   1.25, 1.75, 2,    15),
  ('Jalapenos',         0.5,  0.05,  0.5, 0.75, 1.25, 1.75, 64),
  ('Banana Peppers',    0.5,  0.05,  0.6, 1,    1.3,  1.75, 36),
  ('Regular Cheese',    1.5,  0.12,  2,   3.5,  5,    7,    250),
  ('Four Cheese Blend', 2,    0.15,  2,   3.5,  5,    7,    150),
  ('Feta Cheese',       2,    0.18, 1.75, 3,    4,    5.5,  75),
  ('Goat Cheese',       2,    0.2,   1.6, 2.75, 4,    5.5,  54),
  ('Bacon',             1.5,  0.25,  1,   1.5,  2,    3,    89);

INSERT INTO discount
  (DiscountName, DiscountAmount, DiscountIsPercent)
VALUES 
  ('Employee',            15,   1),
  ('Lunch Special Medium', 1,   0),
  ('Lunch Special Large',  2,   0),
  ('Specialty Pizza',      1.5, 0),
  ('Gameday Special',      20,  1);

INSERT INTO basePriceCost
  (BasePriceCostPizzaSize, BasePriceCostCrustType, BasePriceCostPrice, BasePriceCostCost)
VALUES 
  ('small', 'Thin',         3, 0.5), 
  ('small', 'Original',     3, 0.75), 
  ('small', 'Pan',          3.5, 1), 
  ('small', 'Gluten-Free',  4, 2), 
  ('medium', 'Thin',        5, 1), 
  ('medium', 'Original',    5, 1.5), 
  ('medium', 'Pan',         6, 2.25), 
  ('medium', 'Gluten-Free', 6.25, 3), 
  ('large', 'Thin',         8, 1.25), 
  ('large', 'Original',     8, 2), 
  ('large', 'Pan',          9, 3), 
  ('large', 'Gluten-Free',  9.5, 4), 
  ('x-large', 'Thin',       10, 2), 
  ('x-large', 'Original',   10, 3), 
  ('x-large', 'Pan',        11.5, 4.5), 
  ('x-large', 'Gluten-Free',12.5, 6);

-- ORDER 1 -- 
INSERT INTO pizzaOrder (PizzaOrderType, PizzaOrderCost, PizzaOrderPrice, PizzaOrderTime) 
VALUES ('dine-in', 3.68, 13.50, '2023-03-05 12:03');

SET @orderID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO dineIn VALUES(@orderID, 14);

INSERT INTO pizza (PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES (@orderID, 'Thin', 'large', 13.50, 3.68, 1);

SET @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping (PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES 
  (@pizzaID, 'Regular Cheese', 2), 
  (@pizzaID, 'Pepperoni', 1),
  (@pizzaID, 'Sausage', 1);

INSERT INTO discountToPizza(DiscountToPizzaDiscountID, DiscountToPizzaPizzaID)
VALUES( 
  (SELECT DiscountID FROM discount WHERE DiscountName = 'Lunch Special Large'),
  @pizzaID);


-- ORDER 2 -- 
INSERT INTO pizzaOrder(PizzaOrderType, PizzaOrderCost, PizzaOrderPrice, PizzaOrderTime)
VALUES ('dine-in', 4.63, 17.35, '2023-03-03 12:05');

SET @orderID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO dineIn VALUES (@orderID, 4);

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Pan', 'medium', 10.60, 3.23, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Feta Cheese', 1),
  (@pizzaID, 'Black Olives', 1),
  (@pizzaID, 'Roma Tomato', 1),
  (@pizzaID, 'Mushrooms', 1),
  (@pizzaID, 'Banana Peppers', 1);

INSERT INTO discountToPizza(DiscountToPizzaDiscountID, DiscountToPizzaPizzaID)
VALUES( 
  (SELECT DiscountID FROM discount WHERE DiscountName = 'Lunch Special Medium'), 
  @pizzaID);

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Original', 'small', 6.75, 1.40, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Regular Cheese', 1),
  (@pizzaID, 'Chicken', 1),
  (@pizzaID, 'Banana Peppers', 1);

INSERT INTO discountToPizza(DiscountToPizzaDiscountID, DiscountToPizzaPizzaID)
VALUES( 
  (SELECT DiscountID FROM discount WHERE DiscountName = 'Specialty Pizza'),
  @pizzaID);


-- ORDER 3 -- 
INSERT INTO customer(CustomerName, CustomerPhone)
VALUES ('Ellis Beck', '864-254-5861');

SET @customerID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaOrder(PizzaOrderCustomerID, PizzaOrderType, PizzaOrderCost, PizzaOrderPrice, PizzaOrderTime)
VALUES (@customerID, 'pickup', 19.80, 64.5, '2023-03-03 21:30');

SET @orderID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pickup VALUES (@orderID);

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Original', 'large', 10.75, 3.30, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Regular Cheese', 1),
  (@pizzaID, 'Pepperoni', 1);

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Original', 'large', 10.75, 3.30, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Regular Cheese', 1),
  (@pizzaID, 'Pepperoni', 1);

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Original', 'large', 10.75, 3.30, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Regular Cheese', 1),
  (@pizzaID, 'Pepperoni', 1);

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Original', 'large', 10.75, 3.30, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Regular Cheese', 1),
  (@pizzaID, 'Pepperoni', 1);

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Original', 'large', 10.75, 3.30, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Regular Cheese', 1),
  (@pizzaID, 'Pepperoni', 1);

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Original', 'large', 10.75, 3.30, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Regular Cheese', 1),
  (@pizzaID, 'Pepperoni', 1);


-- ORDER 4 -- 
SET @customerID = (SELECT customerID from customer WHERE customerName='Ellis Beck');

INSERT INTO pizzaOrder (PizzaOrderCustomerID, PizzaOrderType, PizzaOrderCost, PizzaOrderPrice, PizzaOrderTime) 
VALUES (@customerID, 'delivery', 16.86, 45.5, '2023-03-05 19:11');

SET @orderID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO discountToOrder(DiscountToOrderID, DiscountToOrderOrderID)
VALUES( 
  (SELECT DiscountID FROM discount WHERE DiscountName = 'Gameday Special'), 
  @orderID);

INSERT INTO delivery (DeliveryOrderID, DeliveryAddress)
VALUES (@orderID, '115 Party Blvd, Anderson SC 29621');

INSERT INTO pizza (PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES (@orderID, 'Original', 'x-large', 14.50, 5.59, 1);

SET @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping (PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES 
  (@pizzaID, 'Four Cheese Blend', 1),
  (@pizzaID, 'Pepperoni', 1),
  (@pizzaID, 'Sausage', 1);

INSERT INTO pizza (PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES (@orderID, 'Original', 'x-large', 17, 5.59, 1);

SET @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping (PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES 
  (@pizzaID, 'Four Cheese Blend', 1),
  (@pizzaID, 'Ham', 2),
  (@pizzaID, 'Pineapple', 2);

INSERT INTO discountToPizza(DiscountToPizzaDiscountID, DiscountToPizzaPizzaID)
VALUES( 
  (SELECT DiscountID FROM discount WHERE DiscountName = 'Specialty Pizza'),
  @pizzaID);

INSERT INTO pizza (PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES (@orderID, 'Original', 'x-large', 14, 5.68, 1);

SET @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping (PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES 
  (@pizzaID, 'Four Cheese Blend', 1),
  (@pizzaID, 'Jalapenos', 1),
  (@pizzaID, 'Bacon', 1);

-- ORDER 5 -- 
INSERT INTO customer(CustomerName, CustomerPhone)
VALUES ('Kurt McKinney', '864-474-9953');

SET @customerID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaOrder(PizzaOrderCustomerID, PizzaOrderType, PizzaOrderCost, PizzaOrderPrice, PizzaOrderTime)
VALUES (@customerID, 'pickup', 7.85, 16.85, '2023-03-02 17:30');

SET @orderID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pickup VALUES (@orderID);

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Gluten-Free', 'x-large', 16.85, 7.85, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Green Pepper', 1),
  (@pizzaID, 'Onion', 1),
  (@pizzaID, 'Roma Tomato', 1),
  (@pizzaID, 'Mushrooms', 1),
  (@pizzaID, 'Black Olives', 1),
  (@pizzaID, 'Goat Cheese', 1);

INSERT INTO discountToPizza(DiscountToPizzaDiscountID, DiscountToPizzaPizzaID)
VALUES( 
  (SELECT DiscountID FROM discount WHERE DiscountName = 'Specialty Pizza'),
  @pizzaID);

-- ORDER 6 -- 
INSERT INTO customer(CustomerName, CustomerPhone)
VALUES ('Calvin Sanders', '864-232-8944');

SET @customerID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaOrder(PizzaOrderCustomerID, PizzaOrderType, PizzaOrderCost, PizzaOrderPrice, PizzaOrderTime)
VALUES (@customerID, 'delivery', 3.20, 13.25, '2023-03-02 18:17');

SET @orderID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO delivery (DeliveryOrderID, DeliveryAddress)
VALUES(@orderID, '6745 Wessex St Anderson SC 29621');

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Thin', 'large', 13.25, 3.20, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Chicken', 1),
  (@pizzaID, 'Green Pepper', 1),
  (@pizzaID, 'Onion', 1),
  (@pizzaID, 'Mushrooms', 1),
  (@pizzaID, 'Four Cheese Blend', 2);

-- ORDER 7 -- 
INSERT INTO customer(CustomerName, CustomerPhone)
VALUES ('Lance Benton', '864-878-5679');

SET @customerID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaOrder(PizzaOrderCustomerID, PizzaOrderType, PizzaOrderCost, PizzaOrderPrice, PizzaOrderTime)
VALUES (@customerID, 'delivery', 6.30, 24.00, '2023-03-06 20:32');

SET @orderID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO discountToOrder(DiscountToOrderID, DiscountToOrderOrderID)
VALUES( 
  (SELECT DiscountID FROM discount WHERE DiscountName = 'Employee'),
  @orderID);

INSERT INTO delivery (DeliveryOrderID, DeliveryAddress)
VALUES(@orderID, '8879 Suburban Home, Anderson, SC 29621');

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Thin', 'large', 12.00, 3.75, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Four Cheese Blend', 2);

INSERT INTO pizza(PizzaOrderID, PizzaBaseCrustType, PizzaBasePizzaSize, PizzaPrice, PizzaCost, PizzaStatus)
VALUES(@orderID, 'Thin', 'large', 12.00, 2.55, 1);

set @pizzaID = (SELECT DISTINCT LAST_INSERT_ID());

INSERT INTO pizzaTopping(PizzaToppingPizzaID, PizzaToppingName, PizzaToppingQuantity)
VALUES
  (@pizzaID, 'Regular Cheese', 1),
  (@pizzaID, 'Pepperoni', 2);
