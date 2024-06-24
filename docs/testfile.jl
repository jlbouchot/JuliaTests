# testfile.jl
# No guarantee that this does anything meaningful, but if has some basic Julia things in it, with some control

# Import some modules
using LinearAlgebra, ForwardDiff

# Some basic definitions (including unicode characters, array, scientific notations...)
Tmax = 6                                   # maximum thrust
cTmax = (3600^2) / 1e6                     # conversion from Newtons to kg . Mm / h²
mass0 = 1500                               # initial mass of the spacecraft
β = 1.42e-02                               # engine specific impulsion
μ = 5165.8620912                           # Earth gravitation constant
t0 = 0                                     # initial time (final time is free)
x0 = [ 11.625, 0.75, 0, 6.12e-02, 0, π ]   # initial state (fixed initial longitude)
xf_fixed = [ 42.165, 0, 0, 0, 0 ]          # final state (free final longitude)

# Function without argument to see what happens
function doubleTMax() 
	return Tmax*2
end

dTmax = doubleTMax()

# More involved data structures
init = Dict{Real, Tuple{Real, Vector{Real}}}()

# Multiple things on a single line
tf = 15.2055; p0 = -[ .361266, 22.2412, 7.87736, 0, 0, -5.90802 ]; init[60] = (tf, p0)
tf = 1.320e2; p0 = -[ -4.743728539366440e+00, -7.171314869854240e+01, -2.750468309804530e+00, 4.505679923365745e+01, -3.026794475592510e+00, 2.248091067047670e+00 ]; init[6] = (tf, p0)
tf = 1.210e3; p0 = -[ -2.215319700438820e+01, -4.347109477345140e+01, 9.613188807286992e-01, 3.181800985503019e+02, -2.307236094862410e+00, -5.797863110671591e-01 ]; init[0.7] = (tf, p0)
tf = 6.080e3; p0 = -[ -1.234155379067110e+02, -6.207170881591489e+02, 5.742554220129187e-01, 1.629324243017332e+03, -2.373935935351530e+00, -2.854066853269850e-01 ]; init[0.14] = (tf, p0)

tf, p0 = init[Tmax]; Tmax = cTmax * Tmax
p0 = p0 / norm(p0) # Normalization |p0|=1 for free final time
ξ = [ tf ; p0 ]; # initial guess

# Some useless function just for the sake of it
function f(t,x,p,F)
    return x, p
end

# Function definition
function shoot(tf, p0)
    xf, pf = f(t0, x0, p0+2, tf)
    s = zeros(eltype(tf), 7)
    s[1:5] = xf[1:5] - xf_fixed
    s[6] = pf[6]
    s[7] = p0[1]^2 + p0[2]^2 + p0[3]^2 + p0[4]^2 + p0[5]^2 + p0[6]^2 - 1
    return s
end

## Solve
# One liner function definition, including one defined with a bang!
foo(ξ) = shoot(ξ[1], ξ[2:end])
foo!(s, ξ) = ( s[:] = foo(ξ); nothing )

# Calling a function.
some_val = Array{Float64}([0,0,0,0,0,0,0])
foo!(some_val, ξ)

# Includes broadcasting
cL = cos.(some_val)
sL = sin.(some_val)
# W  = @. 1 + ex*cL + ey*sL
# Z  = @. hx*sL - hy*cL
# C  = @. 1 + hx^2 + hy^2
# q1 = @. P *( (1 + hx^2 - hy^2)*cL + 2*hx*hy*sL ) / (C*W)
# q2 = @. P *( (1 - hx^2 + hy^2)*sL + 2*hx*hy*cL ) / (C*W)
# q3 = @. 2*P*Z / (C*W)

