#version 450
#extension GL_ARB_gpu_shader_int64 : require

layout (local_size_x = 256) in;

layout ( binding = 0) buffer buf
{
  float data[];
} block;

float
myexp (float x);

float
mypow (float x, float y);

void main ()
{
  uint gid = gl_GlobalInvocationID.x;
  block.data[gid] = mypow (2.,block.data[gid]);
}

float
mypow (float x, float y)
{
  return myexp (log (x) * y);
}

float 
myexp (float x)
{
  float a = 2.718281828459, result = 1;
  if (x < 0)
  {
    a = 1.0 / a;
    x = -x;
  }

  float aa = a;
  for (uint64_t i = uint64_t(x) ; i != 0; i>>=1, aa *= aa)
    if ( i % 2 == 1)
      result *= aa;

  float rr = 1;
  float _ = 1;

  float f = x - int(x);
  for (int j = 1; j < 14; j++, rr += _)
    _ = _ * f / j;

  return result * rr;
}

