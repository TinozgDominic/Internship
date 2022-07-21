# **Báo cáo thực tập tuần 3**
# Chương trình đọc cảm biến MPU 9250 và điều khiển động cơ không chổi than bằng RaspberryPi Zero

## **Thực tập sinh: Lê Phạm Hoàng Thương** 
### 1. Đọc cảm biến MPU 9250
- MPU 9250 là cảm biến 9 DOF với 3 trục đo gia tốc, 3 trục đo vận tốc góc và 3 trục đo từ trường.
- Cảm biến này có hai chuẩn giao tiếp là I2C và SPI và trong bài này thì dùng I2C.
- Để đọc cảm biến MPU 9250 đầu tiên là phải nối các chân theo sơ đồ sau

    ![alt text](https://images.squarespace-cdn.com/content/v1/59b037304c0dbfb092fbe894/1573589401909-GKK8YB7UJ9FLCCYBDDRP/rpi_mpu9250_wiring_diagram.png?format=1000w)
    *Hình 1: Sơ đồ kết nối*
- Tiếp theo là cấu hình các thanh ghi theo bảng sau
    Địa chỉ cảm biến MPU là 0x68 (đặt là MPU) và cảm biến từ trường là 0x0C (đặt là MAG)
    | Tên thanh ghi| Địa chỉ thanh ghi | Giá trị | Địa chỉ cảm biến |
    | -------------| ----------------- | ------- | ---------------- |
    | SampleRate | 25 | 0x00  | MPU |
    | Configuration| 26 | 0x03  |  MPU |
    | GyroscopeConfiguration |27 | 0x18  |  MPU |
    | AccelerometerConfiguration1 | 28 | 0x18  |  MPU |
    | AccelerometerConfiguration2 | 29 | 0x03  |  MPU |
    | PowerManagement1 | 107 | 0x01  |  MPU |
    | BypassEnableConfiguration | 55 | 0x02  |  MPU |
    | Control1 | 0x0A | 0x16  | MAG |
- Giá trị từ cảm biến đọc được sẽ trả về 16 bit và chi ra 2 thanh ghi nên ta sẽ viết hàm ReadSenSor và hàm ReadMagenetometer (có ở trong file code).
- Khi đọc được giá trị gia tốc ta sẽ tính được PitchFromAcceleration và RollFromAcceleration.
- Từ giá trị con quay hồi chuyển sẽ thu được PitchFromGyroscope, RollFromGyroscope và YawFromGyroscope.
- Để thu được góc Pitch và Roll thì lấy độ tin cậy là 0.02 với Gyroscope và 0.98 tới từ gia tốc như vậy là có Pitch và Roll.
- Từ Cảm biến từ trường và Pitch, Roll ta sẽ tính được thêm góc Yaw, và xác định lại độ tin cậy từ cảm biến từ trường và con quay hồi chuyển cho ra góc Yaw chính xác.
- Vì ở trong môi trường có dòng điện xung quanh sẽ gây nhiễu cho cảm biến từ trường nên ở đây Yaw chỉ tính từ Gyroscope.
- Một số lưu ý khi đọc cảm biến là phải tìm hiểu thông số độ nhạy của cảm biến để tính chính xác chẳng hạn ở bài này thì sau khi đo cảm biến vận tốc góc sẽ chia cho một hệ số 16.4 để trả về giá trị đúng.
- Để cảm biến con quay hồi chuyển đọc tốt hơn có thể thực hiện thêm Offset giá trị. Đầu tiên để cảm biến đứng yên và đo khoảng 100 giá trị của con quay hồi chuyển và sau đó là lấy trung bình 100 giá trị đó ta thu được OffsetX, OffsetY, OffsetZ. Sau khi đọc các giá trị vận tốc góc sẽ trừ thêm các giá trị OffsetX, OffsetY, OffsetZ sẽ giảm thiểu nhiễu.

### 2.Điều khiển động cơ BLDC thông qua ESC và giao thức PWM
- Ta sử dụng thư viện wiringPi để tạo ra xung PWM.
- Chọn bộ chia là 400, Range là 960 ta thu được xung có tần số 50 Hz, là 20ms.
- Ở hàm pwmWrite thì tham số sau có giá trị từ 0 - 1024, nên để tạo xung PWM có độ rộng từ 1ms đến 2ms thì cần tham số từ 52 - 103,

### 2.Điều khiển tốc độ động cơ BLDC dựa trên sự thay đổi về góc Roll
- Ta sẽ tạo một hàm tuyến tính đưa giá trị góc Roll từ -90 đến 90 về giá trị 50 - 100 để đưa vào hàm pwmWrite và thay đổi tốc độ động cơ.

