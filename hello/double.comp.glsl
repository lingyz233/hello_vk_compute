#version 450

layout (local_size_x = 256) in;

layout ( binding = 0) buffer buf
{
  float data[];
} block;

void main ()
{
  uint gid = gl_GlobalInvocationID.x;
  block.data[gid] = sin (exp (block.data[gid] * 10.0f + gid));
}
