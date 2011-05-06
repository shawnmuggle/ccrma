function [a, b] = golay (n)
a = [1];
b = [1];
while n > 0
    n = n - 1;
    
    aResponse = hmeasure(a);
    bResponse = hmeasure(b);
    
    aNext = [a b];
    bNext = [a b*-1];
    a = aNext;
    b = bNext;
end