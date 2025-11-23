const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<meta charset="utf-8">

<head>
  <title>🌿HỆ THỐNG VƯỜN THÔNG MINH🌿</title>
  <style>
    body {
      background-color: rgb(241, 241, 241);
      padding: 10px;
    }
    .container {     
      display: flex;
      justify-content: center; 
      align-items: center; 
      flex-direction: column;
      padding-top: 10px;
      font-family: Monospace;
       
    }
    .button {
      background-color: #04AA6D; /* Green */
      border: none;
      color: white;
      padding: 15px 32px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: x-large;
      margin: 10px 2px;
      cursor: pointer;
      width: 30%;
      border-radius: 5px;
    }
    input[type=text], select ,#caytrong{
      background-color: white;
      width: 100%;
      padding: 12px 20px;
      margin: 8px 0;
      display: inline-block;
      border: 1px solid #ccc;
      border-radius: 4px;
      box-sizing: border-box;
      font-size: xx-large;
      height : 85px;
    }
    input[type=number].tempThre , input[type=number].humiThre , 
    input[type=number].soilThre , input[type=number].dustThre ,
    input[type=number].mq135Thre {
      width: 30%;
      padding: 12px 20px;
      margin: 8px 2px;
      display: inline-block;
      border: 1px solid #ccc;
      border-radius: 4px;
      box-sizing: border-box;
      font-size: xx-large;
      height : 85px;
    }

    .submit {
      width: 100%;
      background-color:#04AA6D;
      color: white;
      padding: 14px 20px;
      margin: 8px 0;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }

    .submit:hover, .button:hover {
      background-color: #989b98;
    }
    .container-2 {
      display: flex;
      gap: 10px;
      margin-bottom: 20px;
    }

    h1 {
      text-align: center;
      margin-bottom: 20px;
    }

    @media (min-width: 300px) and (max-width: 900px) {
      .container {     
        margin-right: 0%;
        margin-left: 0%;
      }
    }
   
  </style>
</head>

<body>
  <div class="container">
      <h1>🌿HỆ THỐNG GIÁM SÁT VƯỜN THÔNG MINH🌿</h1>
      <div>
        <hr>
        <h2>Cấu hình WIFI</h2>
        <div>
          <h4>Tên WIFI </h4>
          <input type="text" id="ssid" name="ssid" placeholder="Your ssid..">
      
          <h4>Mật khẩu</h4>
          <input type="text" id="pass" name="pass" placeholder="Your password ..">

          <h4>Mã Token Blynk</h4>
          <input type="text" id="token" name="token" placeholder="Your Token Blynk ..">

          <h4>Địa chỉ MQTT Server</h4>
          <input type="text" id="mqtt_server" name="mqtt_server" placeholder="Your MQTT Server IP ..">
          
          <hr>

          <h2>Cấu hình ngưỡng</h2>

          <h4>Chọn loại môi trường :</h4>
          <select name="caytrong" id="caytrong" onchange="checkUserSelected()">
            <option value="0">Tự cài đặt</option>
            <option value="1">Trong nhà / Văn phòng</option>
            <option value="2">Nhà máy / Khu công nghiệp</option>
            <option value="3">Ngoài trời</option>
            <option value="4">Bệnh viện / Phòng sạch</option>
            <option value="5">Trường học</option>
            <option value="6">Khu dân cư</option>
            <option value="7">Khu thương mại</option>
          </select>
          <h4>Ngưỡng nhiệt độ môi trường (*C) </h4>
          <p style="font-size: 10px;">🌞Ngưỡng 1 &lt; Khoảng an toàn &lt; Ngưỡng 2</p>
     

          <div class="container-2">
            <input class="tempThre" type="number" id="tempThreshold1" name="tempThreshold1" min="10" max="100" step="1" placeholder="Ngưỡng 1">
            <input class="tempThre" type="number" id="tempThreshold2" name="tempThreshold2" min="10" max="100" step="1" placeholder="Ngưỡng 2">  
          </div>
          
 
          <h4>Ngưỡng độ ẩm không khí (%)</h4>
          <p style="font-size: 10px;">🌱Ngưỡng 1 &lt; Khoảng an toàn &lt; Ngưỡng 2</p>
 
          <div class="container-2">
            <input class="humiThre" type="number" id="humiThreshold1" name="humiThreshold1"  min="10" max="100" step="1" placeholder="Ngưỡng 1">
            <input class="humiThre" type="number" id="humiThreshold2" name="humiThreshold2"  min="10" max="100" step="1"  placeholder="Ngưỡng 2">  
    
          </div>
          


          <h4>Ngưỡng bụi mịn PM2.5 (ug/m3)</h4>
          <p style="font-size: 10px;">🌫️Ngưỡng 1 &lt; Khoảng an toàn &lt; Ngưỡng 2</p>
 
          <div class="container-2">
            <input class="dustThre" type="number" id="dustThreshold1" name="dustThreshold1" min="0" max="500" step="1" placeholder="Ngưỡng 1">
            <input class="dustThre" type="number" id="dustThreshold2" name="dustThreshold2" min="0" max="500" step="1" placeholder="Ngưỡng 2">  
          </div>

          <h4>Ngưỡng MQ135 Gas Ratio</h4>
          <p style="font-size: 10px;">💨Ngưỡng 1 &lt; Khoảng an toàn &lt; Ngưỡng 2</p>
 
          <div class="container-2">
            <input class="mq135Thre" type="number" id="mq135Threshold1" name="mq135Threshold1" min="0" max="100" step="0.1" placeholder="Ngưỡng 1">
            <input class="mq135Thre" type="number" id="mq135Threshold2" name="mq135Threshold2" min="0" max="100" step="0.1" placeholder="Ngưỡng 2">  
          </div>

      
          <button class="submit" id="btnSubmit"><h4 style="font-size: 35px;">Gửi</h4></button>
        </div>
      </div>
      <div>
         
      </div>
    
  </div>

  <script>


    var data = {
          ssid   : "",
          pass   : "",
          token : "",
          mqtt_server : "",
          typePlant : "",
          tempThreshold1 : "",
          tempThreshold2 : "",
          humiThreshold1 : "",
          humiThreshold2 : "",
          dustThreshold1 : "",
          dustThreshold2 : "",
          mq135Threshold1 : "",
          mq135Threshold2 : "",
    };
     const ssid        = document.getElementById("ssid");
     const pass        = document.getElementById("pass");
     const token       = document.getElementById("token");
     const mqtt_server = document.getElementById("mqtt_server");
     const typePlant   = document.getElementById("caytrong");

    const tempThreshold1 =  document.getElementsByName('tempThreshold1')[0];
    const tempThreshold2 =  document.getElementsByName('tempThreshold2')[0];

    const humiThreshold1 = document.getElementsByName('humiThreshold1')[0];
    const humiThreshold2 = document.getElementsByName('humiThreshold2')[0];

    const dustThreshold1 = document.getElementsByName('dustThreshold1')[0];
    const dustThreshold2 = document.getElementsByName('dustThreshold2')[0];

    const mq135Threshold1 = document.getElementsByName('mq135Threshold1')[0];
    const mq135Threshold2 = document.getElementsByName('mq135Threshold2')[0];

    // Khởi tạo các giá trị ngưỡng của môi trường
    // Format: PLANT(tempThreshold1, tempThreshold2, humiThreshold1, humiThreshold2, dustThreshold1, dustThreshold2, mq135Threshold1, mq135Threshold2)
    function PLANT(tempThreshold1, tempThreshold2, humiThreshold1, humiThreshold2, dustThreshold1, dustThreshold2, mq135Threshold1, mq135Threshold2) {
        this.tempThreshold1 = tempThreshold1;
        this.tempThreshold2 = tempThreshold2;
        this.humiThreshold1 = humiThreshold1;
        this.humiThreshold2 = humiThreshold2;
        this.dustThreshold1 = dustThreshold1;
        this.dustThreshold2 = dustThreshold2;
        this.mq135Threshold1 = mq135Threshold1;
        this.mq135Threshold2 = mq135Threshold2;
    }
    // 1. Trong nhà / Văn phòng: Nhiệt độ 20-25°C, Độ ẩm 40-60%, PM2.5 0-35, MQ135 0-1.0
    trongnha = new PLANT(20, 25, 40, 60, 0, 35, 0, 1.0);
    // 2. Nhà máy / Khu công nghiệp: Nhiệt độ 15-30°C, Độ ẩm 30-70%, PM2.5 0-75, MQ135 0-2.0
    nhamay = new PLANT(15, 30, 30, 70, 0, 75, 0, 2.0);
    // 3. Ngoài trời: Nhiệt độ -10-40°C, Độ ẩm 20-90%, PM2.5 0-50, MQ135 0-1.5
    ngoaitroi = new PLANT(-10, 40, 20, 90, 0, 50, 0, 1.5);
    // 4. Bệnh viện / Phòng sạch: Nhiệt độ 22-24°C, Độ ẩm 45-55%, PM2.5 0-15, MQ135 0-0.5
    benhvien = new PLANT(22, 24, 45, 55, 0, 15, 0, 0.5);
    // 5. Trường học: Nhiệt độ 20-26°C, Độ ẩm 40-60%, PM2.5 0-35, MQ135 0-1.0
    truonghoc = new PLANT(20, 26, 40, 60, 0, 35, 0, 1.0);
    // 6. Khu dân cư: Nhiệt độ 18-28°C, Độ ẩm 40-70%, PM2.5 0-50, MQ135 0-1.2
    khudancu = new PLANT(18, 28, 40, 70, 0, 50, 0, 1.2);
    // 7. Khu thương mại: Nhiệt độ 20-26°C, Độ ẩm 40-65%, PM2.5 0-40, MQ135 0-1.5
    khuthuongmai = new PLANT(20, 26, 40, 65, 0, 40, 0, 1.5);
    
    // lấy data ban đầu
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET","/data_before", true),
    xhttp.send();
    xhttp.onreadystatechange = function() {
      if(xhttp.readyState == 4 && xhttp.status == 200) {
        //alert(this.responseText);
        const obj    = JSON.parse(this.responseText); // chuyển JSON sang JS Object
        //alert(obj.ssid);
        ssid.value   = obj.ssid;
        pass.value   = obj.pass;
        token.value  = obj.token;
        mqtt_server.value = obj.mqtt_server;
        typePlant.value = obj.typePlant;
        tempThreshold1.value = obj.tempThreshold1;
        tempThreshold2.value = obj.tempThreshold2;
        humiThreshold1.value = obj.humiThreshold1;
        humiThreshold2.value = obj.humiThreshold2;
        dustThreshold1.value = obj.dustThreshold1;
        dustThreshold2.value = obj.dustThreshold2;
        mq135Threshold1.value = obj.mq135Threshold1;
        mq135Threshold2.value = obj.mq135Threshold2;

        // Gán giá trị môi trường ban đầu
        document.getElementById("caytrong").options.selectedIndex = Number(obj.typePlant);
        checkPlantAndPutValue(Number(obj.typePlant));
      }
    }

   
    
    // Hàm check kiểu cây trồng
    function checkUserSelected() {
      var index = typePlant.options.selectedIndex;
      checkPlantAndPutValue(index);
    }

    // hiển thị các giá trị ngưỡng tương ứng lên textbox khi người dùng chọn loại môi trường
    function checkPlantAndPutValue(value) {
        if(value === 0) {
          document.getElementById("tempThreshold1").disabled = false;
          document.getElementById("tempThreshold2").disabled = false;
          document.getElementById("humiThreshold1").disabled = false;
          document.getElementById("humiThreshold2").disabled = false;
          document.getElementById("dustThreshold1").disabled = false;
          document.getElementById("dustThreshold2").disabled = false;
          document.getElementById("mq135Threshold1").disabled = false;
          document.getElementById("mq135Threshold2").disabled = false;
        } else {
          document.getElementById("tempThreshold1").disabled = true;
          document.getElementById("tempThreshold2").disabled = true;
          document.getElementById("humiThreshold1").disabled = true;
          document.getElementById("humiThreshold2").disabled = true;
          document.getElementById("dustThreshold1").disabled = true;
          document.getElementById("dustThreshold2").disabled = true;
          document.getElementById("mq135Threshold1").disabled = true;
          document.getElementById("mq135Threshold2").disabled = true;
          switch (value) {
            case 1: // Trong nhà / Văn phòng
              tempThreshold1.value = trongnha.tempThreshold1;
              tempThreshold2.value = trongnha.tempThreshold2;
              humiThreshold1.value = trongnha.humiThreshold1;
              humiThreshold2.value = trongnha.humiThreshold2;
              dustThreshold1.value = trongnha.dustThreshold1;
              dustThreshold2.value = trongnha.dustThreshold2;
              mq135Threshold1.value = trongnha.mq135Threshold1;
              mq135Threshold2.value = trongnha.mq135Threshold2;
              break;
            case 2: // Nhà máy / Khu công nghiệp
              tempThreshold1.value = nhamay.tempThreshold1;
              tempThreshold2.value = nhamay.tempThreshold2;
              humiThreshold1.value = nhamay.humiThreshold1;
              humiThreshold2.value = nhamay.humiThreshold2;
              dustThreshold1.value = nhamay.dustThreshold1;
              dustThreshold2.value = nhamay.dustThreshold2;
              mq135Threshold1.value = nhamay.mq135Threshold1;
              mq135Threshold2.value = nhamay.mq135Threshold2;
              break;
            case 3: // Ngoài trời
              tempThreshold1.value = ngoaitroi.tempThreshold1;
              tempThreshold2.value = ngoaitroi.tempThreshold2;
              humiThreshold1.value = ngoaitroi.humiThreshold1;
              humiThreshold2.value = ngoaitroi.humiThreshold2;
              dustThreshold1.value = ngoaitroi.dustThreshold1;
              dustThreshold2.value = ngoaitroi.dustThreshold2;
              mq135Threshold1.value = ngoaitroi.mq135Threshold1;
              mq135Threshold2.value = ngoaitroi.mq135Threshold2;
              break;
            case 4: // Bệnh viện / Phòng sạch
              tempThreshold1.value = benhvien.tempThreshold1;
              tempThreshold2.value = benhvien.tempThreshold2;
              humiThreshold1.value = benhvien.humiThreshold1;
              humiThreshold2.value = benhvien.humiThreshold2;
              dustThreshold1.value = benhvien.dustThreshold1;
              dustThreshold2.value = benhvien.dustThreshold2;
              mq135Threshold1.value = benhvien.mq135Threshold1;
              mq135Threshold2.value = benhvien.mq135Threshold2;
              break;
            case 5: // Trường học
              tempThreshold1.value = truonghoc.tempThreshold1;
              tempThreshold2.value = truonghoc.tempThreshold2;
              humiThreshold1.value = truonghoc.humiThreshold1;
              humiThreshold2.value = truonghoc.humiThreshold2;
              dustThreshold1.value = truonghoc.dustThreshold1;
              dustThreshold2.value = truonghoc.dustThreshold2;
              mq135Threshold1.value = truonghoc.mq135Threshold1;
              mq135Threshold2.value = truonghoc.mq135Threshold2;
              break;
            case 6: // Khu dân cư
              tempThreshold1.value = khudancu.tempThreshold1;
              tempThreshold2.value = khudancu.tempThreshold2;
              humiThreshold1.value = khudancu.humiThreshold1;
              humiThreshold2.value = khudancu.humiThreshold2;
              dustThreshold1.value = khudancu.dustThreshold1;
              dustThreshold2.value = khudancu.dustThreshold2;
              mq135Threshold1.value = khudancu.mq135Threshold1;
              mq135Threshold2.value = khudancu.mq135Threshold2;
              break;
            case 7: // Khu thương mại
              tempThreshold1.value = khuthuongmai.tempThreshold1;
              tempThreshold2.value = khuthuongmai.tempThreshold2;
              humiThreshold1.value = khuthuongmai.humiThreshold1;
              humiThreshold2.value = khuthuongmai.humiThreshold2;
              dustThreshold1.value = khuthuongmai.dustThreshold1;
              dustThreshold2.value = khuthuongmai.dustThreshold2;
              mq135Threshold1.value = khuthuongmai.mq135Threshold1;
              mq135Threshold2.value = khuthuongmai.mq135Threshold2;
              break;
          }
        }
    }

    // Gửi data về server khi click vào nút Submit
    var xhttp2 = new XMLHttpRequest();
    const btnSubmit = document.getElementById("btnSubmit"); 
    btnSubmit.addEventListener('click', () => { 
        data = {
          ssid   : ssid.value,
          pass   : pass.value,
          token  :  token.value,
          mqtt_server : mqtt_server.value,
          typePlant :  Number(typePlant.value),
          tempThreshold1 : Number(tempThreshold1.value),
          tempThreshold2 : Number(tempThreshold2.value),
          humiThreshold1 : Number(humiThreshold1.value),
          humiThreshold2 : Number(humiThreshold2.value),
          dustThreshold1 : Number(dustThreshold1.value),
          dustThreshold2 : Number(dustThreshold2.value),
          mq135Threshold1 : Number(mq135Threshold1.value),
          mq135Threshold2 : Number(mq135Threshold2.value)
        }
        
        xhttp2.open("POST","/post_data", true),
        xhttp2.send(JSON.stringify(data)); // chuyển JSObject sang JSON để gửi về server
        xhttp2.onreadystatechange = function() {
          if(xhttp2.readyState == 4 && xhttp2.status == 200) {
            alert("Cài đặt thành công");
          } 
        }
    });

  </script>
  
</body>

</html>
)rawliteral";

