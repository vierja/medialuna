-- BINARY
print("BINARY")
-- OR
print("OR")
a = 10 or 20
print(a, "==", 10)
print(10 or error(), "==", 10)
print(nil or "a", "==", "a")
print(false or nil, "==", nil)

-- AND
print("AND")
print(nil and 10, "==", nil)
print(false and error(), "==", false)
print(false and nil, "==", false)
print(10 and 20, "==", 20)

-- ==
print("==")
print((nil == nil))
print(false == false)
print((false == true) == false)
print("a" == "a")
print(2 == 2)
print(2 == 2.0)

-- ~=
print("~=")
print(nil ~= "b")
print("b" ~= "a")
print(1 ~= 2)

-- ..
print("..")
print("Ho".."la")
print(1 .. 2.0)
print(1 .. " = uno.")

-- +
print("+")
print(1 + 2);
print(1.2 + 2);
print("1" + 1);
print("2" + "2");
print(1.2 + 1.2);

-- -
print("-")
print(1 - 2);
print("1" - 1);
print("2" - "5");
print(1.2 - 1.2);

-- *
print("*")
print(1 * 1);
print("1" * 3);
print("4" * "4");
print("1.3" * 1);
print(1.1 * 1.2);


-- /
print("/")
print(1 / 1);
print("1" / 3);
print("1" / "2");
print("1" / 0.5);
print(1.1 / 1.2);

-- %
print("%")
print(1 % 1);
print("1" % 3);
print("10" % "2");
print("1" % 0.3);
print(1.1 % 1.2);

-- ^
print("^")
print(1 ^ 1);
print("1" ^ 3);
print("10" ^ "2");
print("1" ^ 0.3);
print(1.1 ^ 1.2);

-- UNARY
print("UNARY")
-- -
print("-")
print(-"1");
print(-"-10"); -- TODO : ANDA MAL.
print(-10);

-- not
print("not")
print(not nil)
print(not false)
print(not true)
print(not "a")
