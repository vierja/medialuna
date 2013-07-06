function get_all_factors(number, shit)

    local factors = {}
    for possible_factor=1, math.sqrt(number), 1 do
        local remainder = number%possible_factor
        
        if remainder == 0 then
            local factor, factor_pair = possible_factor, number/possible_factor
            table.insert(factors, factor)
            
            if factor ~= factor_pair then
                table.insert(factors, factor_pair)
            end
        end
    end
    
    table.sort(factors)
    return factors
end

a = 10
b = a
a = 1
print("a" or false)

--The Meaning of the Universe is 42. Let's find all of the factors driving the Universe.

local the_universe = 42
factors_of_the_universe = get_all_factors(the_universe)

print("hola")
print(the_universe, factors_of_the_universe)

--Print out each factor

print("Count",  "The Factors of Life, the Universe, and Everything")
table.foreach(factors_of_the_universe, print)