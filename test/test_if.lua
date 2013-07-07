a = "hola"

if a == "hola" then
    print("La variable a = " .. a)
end

if false then
    print("NO APARECE")
end

if nil then
    print("NO aparece")
end

if a ~= "hola" then
    print("FUCK")
elseif a == "chau" then
    print("SHITTT")
elseif a == "sayonara" then
    print("FUUUUUUCK")
else
    print("TODO BIEN")
end

function iguales(primero, segundo)
    if primero == segundo then
        return primero
    else
        return segundo
    end
end

b = iguales(1, 2)
print("Funcion(1,2) = 2 == " .. b)