#version 450

layout (local_size_x = 256) in;

layout ( binding = 0) buffer buf
{
  float data[];
} block;

void main ()
{
  uint gid = gl_GlobalInvocationID.x;
  block.data[gid] *= 2.0f;
}
