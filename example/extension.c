// The structure which defines the version, functions, and data for the extension:
typedef struct hsa_ext_acme_foo_s {
    // Major version number of the extension.
    int major_version;
    // Minor version number of the extension.
    int minor_version;
    // Function pointers:
    int (*function1) ( int p1, int *p2, float p3, int p4);
    int (*function2) ( int* p1, int p2);
    // Data:
    unsigned foo_data1;
} hsa_ext_acme_foo_t;

main() {
    struct hsa_ext_acme_foo_t acmeFoo;
    hsa_status_t status = hsa_vendor_extension_query(HSA_EXT_ACME_FOO, &acmeFoo);
    if (status == HSA_STATUS_SUCCESS) {
        (*(acmeFoo.function2))(0, 0);
    }
}
