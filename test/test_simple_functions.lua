function print_magia(magia)
    print(magia)
end

print_magia("Magia")
print_magia("Magia2", "Chau")
print_magia()

function devuelvo_mismo_valor(valor)
    print("Voy a devolver el valor ".. valor)
    return valor
end

a = devuelvo_mismo_valor(1)
print("El valor de a es: " .. a)

function muchos_val()
    return 1, 2, 3
end

print(muchos_val()) -- imprime 1    2   3
print(1, muchos_val()) -- imprime 1     1
