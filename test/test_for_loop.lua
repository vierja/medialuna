-- 1, 2, 3, 4, 5
for a = 1, 5 do
    print(a)
end

-- 2, 4, 6
for a = 2, 6, 2 do
    print(a)
end

-- nada
for a = 0, -5 do
    print(a)
end

-- - 0, -1, -2, -3, -4, -5
for a = 0, -5, -1 do
    print(a)
end

-- 0
for a = 0, 5 do
    print(a)
    break
end

-- 0
for a = 0, 5 do
    print(a)
    return
end