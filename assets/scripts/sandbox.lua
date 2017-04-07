ffi = require('ffi')

---------------------------------------------------------

ffi.cdef[[
void LogDebug(const char *msg);
void LogError(const char *msg);
void LogInfo(const char *msg);
void LogWarn(const char *msg);
]]

debug = function(fmt, ...)
		ffi.C.LogDebug(fmt:format(...))
	end
error = function(fmt, ...)
		ffi.C.LogError(fmt:format(...))
	end
info = function(fmt, ...)
		ffi.C.LogInfo(fmt:format(...))
	end
warn = function(fmt, ...)
		ffi.C.LogWarn(fmt:format(...))
	end

---------------------------------------------------------

--[[
ffi.cdef[[
typedef struct vec2
{
	float x;
	float y;
} vec2;
]]

ffi.cdef[[
typedef struct vec4
{
	float x;
	float y;
	float z;
	float w;
} vec4;
]]

ffi.cdef[[
typedef struct uiScissorRule
{
	vec4 scissorLocal;
	vec4 scissorAbsolute;
	vec4 scissorPadding;
} uiScissorRule;
]]

ffi.cdef[[
typedef struct uiControlTest
{
	vec2 positionNormalized;
	vec2 positionLocal;
	vec2 positionAbsolute;
	vec2 extentNormalized;
	vec2 extent;
	uiScissorRule *scissor;
	vec4 color;
} uiControlTest;
]]

local ctrl = ffi.new("uiControlTest")
ctrl.positionLocal.x = 0.1
info("%f", ctrl.positionLocal.x)
testScissorA = ffi.new("uiScissorRule")
testScissorA.scissorLocal.x = 0.5
testScissorB = ffi.new("uiScissorRule")
testScissorB.scissorLocal.x = 0.75
ctrl.scissor = testScissorA
info("%f", ctrl.scissor.scissorLocal.x)
ctrl.scissor = testScissorB
info("%f", ctrl.scissor.scissorLocal.x)
]]




----------------------------

function require_intercept(a)
  if (a == "ffi" or a == "bit") then
    return nil
  end
  return require(a)
end

sandbox_env = {
  ipairs = ipairs,
  next = next,
  pairs = pairs,
  pcall = pcall,
  tonumber = tonumber,
  tostring = tostring,
  type = type,
  unpack = unpack,
  coroutine = { create = coroutine.create, resume = coroutine.resume, 
      running = coroutine.running, status = coroutine.status, 
      wrap = coroutine.wrap },
  string = { byte = string.byte, char = string.char, find = string.find, 
      format = string.format, gmatch = string.gmatch, gsub = string.gsub, 
      len = string.len, lower = string.lower, match = string.match, 
      rep = string.rep, reverse = string.reverse, sub = string.sub, 
      upper = string.upper },
  table = { insert = table.insert, maxn = table.maxn, remove = table.remove, 
      sort = table.sort },
  math = { abs = math.abs, acos = math.acos, asin = math.asin, 
      atan = math.atan, atan2 = math.atan2, ceil = math.ceil, cos = math.cos, 
      cosh = math.cosh, deg = math.deg, exp = math.exp, floor = math.floor, 
      fmod = math.fmod, frexp = math.frexp, huge = math.huge, 
      ldexp = math.ldexp, log = math.log, log10 = math.log10, max = math.max, 
      min = math.min, modf = math.modf, pi = math.pi, pow = math.pow, 
      rad = math.rad, random = math.random, sin = math.sin, sinh = math.sinh, 
      sqrt = math.sqrt, tan = math.tan, tanh = math.tanh },
  os = { clock = os.clock, difftime = os.difftime, time = os.time },

  debug = debug,
  error = error,
  info = info,
  print = info,
  warn = warn,

  require = require_intercept,
}

setfenv(0,sandbox_env)