#define	DIO_INPUT		1
#define	DIO_OUTPUT		0
#define	DIO_HIGH		1
#define	DIO_LOW			0
#define	IOCTL_DIO_GET_MODE	1
#define	IOCTL_DIO_SET_MODE	2
#define	IOCTL_DIO_GET_DATA	3
#define	IOCTL_DIO_SET_DATA	4
#define	IOCTL_SET_DOUT		15
#define	IOCTL_GET_DOUT		16
#define	IOCTL_GET_DIN		17
#define	ALL_PORT		-1

struct dio_set_struct {
	int	io_number;
	int	mode_data;// 1 for input, 0 for output, 1 for high, 0 for low
};
