# hello_vk_compute

### it doesn't work as hoped

it generates 0, 1, 2 ... to the shader
```C
  float * data = malloc (sizeof (float) * CNT);
  for (int i = 0; i < CNT; i++)
    data[i] = i;

  float * mapaddr;
  if (vkMapMemory (device, devmem, 0, sizeof (float) * CNT, 0, (void**)&mapaddr) != VK_SUCCESS)
    errx (-1, "(%s) failed to map memory\n", __FUNCTION__);
  memcpy (mapaddr, data, sizeof (float) * CNT);
  vkUnmapMemory (device, devmem);
```

and hoped to double the data, in which the result is hoped to be 0, 2, 4, 6, ..., as in the double.comp.glsl:
```glsl
#version 450

layout (local_size_x = 256) in;

layout (set = 0, binding = 0) buffer StorageBuffer
{
  float data[];
} block;

void main ()
{
  uint gid = gl_GlobalInvocationID.x;
  block.data[gid] *= 2.0f;
}
```

and then print the data out:
```C
  if (vkMapMemory (device, devmem, 0, sizeof (float) * CNT, 0, (void**)&mapaddr) != VK_SUCCESS)
    errx (-1, "(%s) failed to map memory\n", __FUNCTION__);
//  memcpy (data, mapaddr, sizeof (float) * 1024);

  for (int i = 0; i < CNT; i++)
    printf ("%.1f \t", mapaddr[i]);

  vkUnmapMemory (device, devmem);
```

however it outputs:
```
0.0 	1.0 	2.0 	3.0 	4.0 	5.0 	6.0 	7.0 	8.0 	9.0 	10.0 	11.0 	12.0  ...
```
