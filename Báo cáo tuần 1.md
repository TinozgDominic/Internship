# **Báo cáo thực tập tuần 1**

## **Thực tập sinh: Lê Phạm Hoàng Thương** 
### 1. Động cơ được dùng trong Drone
- Động cơ được sử dụng trong việc chế tạo Drone là động cơ không chổi than (Brushless DC Motor - BLDC).
- Nguyên lý của động cơ không chổi than dựa trên lực từ giữa nam châm vĩnh cửu và từ trường tạo bời các cuộn dây có trong động cơ.

    ![alt text](https://cuatudonganhkhoa.com/wp-content/uploads/2022/02/brushless-motor-1.jpg)

    *Hình 1: Động cơ không chổi than*.
- Ta sẽ điều khiển động cơ không chổi than thông qua mạch ESC (Electronic Speed Controllers) chứ không trực tiếp lập trình để điều khiển động cơ.

### 2. ESC
- Cách kết nối ESC với FC và BLDC được mô tả như hình sau

    ![alt text](https://howtomechatronics.com/wp-content/uploads/2019/02/Arduino-Brushelss-Motor-Control-using-ESC.png)

    *Hình 2: Cách kết nối ESC*.

    Trong đó: cụm ba chân đỏ - cam - nâu - là cụm chân nhận tín hiệu.
- Muốn sử dụng ESC ta phải lập trình trên ESC Firmware như BLHeli32 ESCs hay BLHeli_S ESCs có trên ArduPilot. Khi đó ta sẽ cấu hình cho ESC theo các giao thức mong muốn. 

### 3. Giao thức kết nối giữ FC và ESC
#### 3.1. PWM
- Đây là giao thức mà ta dùng xung PWM có độ rộng từ 1000 us tới 2000 us ứng với tốc độ thấp nhất và tốc độ cao nhất của động cơ. 
- Độ trễ tối đa của giao thức này là 2 ms, gây khó khăn cho việc đồng bộ với bộ điều khiển PID
#### 3.2. Oneshot
- Bởi vì PWM thông thường khá là chậm nên người ta đã phát triển ra Oneshot
- Hiện nay có hai loại Oneshot thông dụng là:
    + Oneshot125: với độ rộng xung từ 125 us đến 250 us cho tốc độ cập nhật nhanh gấp 8 lần so với PWM thông thường.
    + Oneshot42: với dộ rộng xung từ 42 us đến 84 us và tốc độ cập nhật nhanh gấp 3 lần so với Oneshot125.
- Tốc độ cập nhật có nghĩa là thời gian tối thiểu để ESC thay đổi tốc độ của BLDC.
#### 3.3. Multishot 
- Multishot là những xung có độ rộng từ 5us đến 25us cho tốc độ cập nhật nhanh gấp 10 lần Oneshot125.
#### 3.4. Dshot 
- Tuy Multishot có tốc độ nhanh nhưng các giao thức như Multishot, Oneshot và PWM đều là các giao thức Analog. Có nghĩa là phải dựa vào độ rộng của xung mới xác định được tốc độ. Mà giao thức Analog thường có vấn đề về nhiễu và phải có sự hiệu chỉnh để đưa từ độ rộng xung về tốc độ động cơ.
- Do đó người ta đã phát triển lên Dshot là một giao thức Digital. Và bởi vì là giao thức Digital có nghĩa tín hiệu chỉ là 0 và 1 nên sẽ không cần hiệu chỉnh cũng như không lo lắng về vấn đề nhiễu.
- Dshot là một chuỗi dữ liệu gồm 16 bit gồm:
    - 11 bit giá trị tốc độ (2048 cấp).
    - 1 bit yêu cầu đo từ xa (telementry request).
    - 4 bit cho kiểm tra CRC.
- Các loại Dshot:
    - DShot150 : tốc độ 150  kb/s, tốc độ cập nhật 106.8 us.
    - DShot300 : tốc độ 300  kb/s, tốc độ cập nhật 53.4  us.
    - DShot600 : tốc độ 600  kb/s, tốc độ cập nhật 26.7  us.
    - DShot1200: tốc độ 1200 kb/s, tốc độ cập nhật 13.4  us.

![alt text](https://oscarliang.com/ctt/uploads/2017/05/dshot1200-esc-protocol-speed-bitrate-latency.jpg)

*Hình 3: So sánh các giao thức*.
