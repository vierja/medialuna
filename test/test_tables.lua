t = {}
table.foreach(t, print)
t["foo"] = 123
t["jaja"] = "bar"
print("********")
table.foreach(t, print)
t = {}
t[1] = 123
t[2] = "bar"
print("********")
table.foreach(t, print)
t = {foo = "bar", [123] = 456}
print("********")
table.foreach(t, print)
t = {"a", "b", "c"}
print("********")
table.foreach(t, print)
t = {[1]="a", [2]="b", [3]="c"}
print("********")
table.foreach(t, print)
t[1] = "d"
print("********")
table.foreach(t, print)
t = {"a", "b", [123]="foo", "c", name="bar", "d", "e"}
print("********")
table.foreach(t, print)
t = {"a", "c"}
table.insert(t, "b")
print("********")
table.foreach(t, print)
print("********")
for i, v in pairs(t) do print(i, v) end
for i in pairs(t) do print(i) end
