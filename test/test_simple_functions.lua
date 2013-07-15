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

function has_inside_function(a)
    b = a/2
    function inside(a)
        print("Estoy adentro")
        return a/2
    end
    c = inside(b)
    return c
end

c = has_inside_function(4)
print(c)

function funcion1(a)
    function funcion2(a)
        function funcion3(a)
            function funcion4(a)
                return a * 2
            end
            return funcion4(a) * 2
        end
        return funcion3(a) * 2
    end
    return funcion2(a) * 2
end
--32
print(funcion1(2))