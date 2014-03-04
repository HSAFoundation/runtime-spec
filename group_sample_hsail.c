kernel &myDynamicGroupMemKernel (
                kernarg_u32 %groupOffset0,
                kernarg_u32 %groupOffset1,
                kernarg_u32 %groupOffset2,
                kernarg_u32 %foo)
{
        ld_kernarg_u32 $s0, [%groupOffset0]
        workitemid        $s1
        add               $s2, $s1, $s0
        st_group       0, [$s2]
        barrier
        ...
