#include <stdio.h>

#include <hidapi/hidapi.h>


int main(void)
{
	int res = hid_init();
	if(res)
	{
		printf("Error with HidAPI !");
		return -1;
	}

	struct hid_device_info *devs, *cur_dev;
	
    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;

    while (cur_dev)
    {
    	printf("====Device====\n Vendor ID: %04hx\n Product ID: %04hx\n Path: %s\n Mac Address: %ls\n",
			cur_dev->vendor_id,
            cur_dev->product_id,
            cur_dev->path,
            cur_dev->serial_number);
		printf(" Manufacturer Name: %ls\n", cur_dev->manufacturer_string);
		printf(" Product Name:      %ls\n", cur_dev->product_string);
        printf("==============\n");
		cur_dev = cur_dev->next;
    }
    
	hid_free_enumeration(devs);

    return 0;    
}
