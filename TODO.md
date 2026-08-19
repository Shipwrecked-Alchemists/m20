| Component     | Status | Description     |
|---------------|--------|-----------------|
| GPS M10M      | V      | POS GS TIME     |
| LPS22HB B&T   | V      | Press Temp      |
| Vbat          | V      | Battery Voltage |
| Radio AD7012B | X      |                 |
| Humidity      | V      |                 |
| NTC           | V      | Need validation |
| IR            | X      |                 |
| UART          | V      |                 |

```c
typedef struct m20_frame {
    uint8_t len;
    uint8_t seq;

    uint32_t time; // Unix timestamp

    uint32_t lat;
    uint32_t lon;
    uint16_t alt; // 1e-1m

    uint16_t vel_h; // Horizontal velocity
    uint16_t vel_v; // Vertical velocity
    uint8_t heading; // Heading @2deg

    uint16_t temp;
    uint16_t humidity;

    uint16_t crc;
} m20_frame_t;
```

## NTC

Using Steinhart–Hart equation

```
1/T = A + B*log(R) + C*log(R)^2 + D*log(R)^3

A = 0.0010730351
B = 0.0002412940
C = 0.0000022675
D = 0.0000000653
```