t = {"a", "b"}
t[1] = "c"
table.insert(t, "d")
table.foreach(t, print)
