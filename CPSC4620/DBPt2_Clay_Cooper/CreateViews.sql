/*
    Zach Clay and Ashlyn Cooper
*/

USE PizzasRUs;

-- Topping Popularity --
CREATE OR REPLACE VIEW ToppingPopularity AS 
SELECT
	PizzaToppingName AS 'Topping',
    SUM(PizzaToppingQuantity) AS 'ToppingCount'
FROM
	pizzaTopping
GROUP BY
	PizzaToppingName
ORDER BY 
    ToppingCount DESC;

SELECT * FROM ToppingPopularity;

-- Profit By Pizza -- 
CREATE OR REPLACE VIEW ProfitByPizza AS 
SELECT
    p.PizzaBaseCrustType AS 'Pizza Crust',
    p.PizzaBasePizzaSize AS 'Pizza Size',
    SUM(p.PizzaPrice - p.PizzaCost) AS 'Profit',
    DATE_FORMAT(MAX(o.PizzaOrderTime), "%M-%d-%Y") AS 'LastOrderDate'
FROM pizza p
	INNER JOIN pizzaOrder o ON p.PizzaOrderID=o.PizzaOrderID
GROUP BY 
    p.PizzaBasePizzaSize, p.PizzaBaseCrustType
ORDER BY 
    Profit DESC;

SELECT * FROM ProfitByPizza;

-- Profit By Order Type --
CREATE OR REPLACE VIEW ProfitByOrderType AS 
SELECT 
	PizzaOrderType AS 'CustomerType',
    DATE_FORMAT(PizzaOrderTime, "%Y-%M") AS 'OrderMonth',
    PizzaOrderPrice AS 'TotalOrderPrice',
    PizzaOrderCost AS 'TotalOrderCost',
    (PizzaOrderPrice - PizzaOrderCost) AS 'Profit'
FROM pizzaOrder
UNION 
SELECT
	'' PizzaOrderType,
    'Grand Total' PizzaOrderTime, 
    SUM(PizzaOrderPrice),
    SUM(PizzaOrderCost),
    SUM(PizzaOrderPrice - PizzaOrderCost)
FROM pizzaOrder;


SELECT * FROM ProfitByOrderType;