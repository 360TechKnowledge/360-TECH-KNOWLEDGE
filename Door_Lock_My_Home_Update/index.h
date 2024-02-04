const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
   <style type="text/css">
      .button {
      background-color: #0319F7; /* BLUE */
      border: none;
      color: white;
      padding: 15px 32px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      }
   </style>
   <title>Neways International</title>
   <link rel="icon" href="http://erp.superhomebd.com/super_home/assets/img/favicon.png" type="image/gif" sizes="16x16">
   <center>
      <div>
         <img src="http://erp.superhomebd.com/super_home/assets/img/neways.png" style="width: 150px;padding-top: 15px;" alt="Super Hostel" title="Super Hostel">
         <h1>
            <marquee>Neways Universal Circuit</marquee>
         </h1>
         <div id = "state">
         
         </div>
         <button class="button" onclick="send(1)">Open the door</button>
      </div>
      <br>
      <script>
         function send(lock_sts) {
           var xhttp = new XMLHttpRequest();
           xhttp.onreadystatechange = function() {
             if (this.readyState == 4 && this.status == 200) {
                 //document.getElementById("state").innerHTML = 
                 this.responseText;
//                 console.log(this.responseText);
             }
           };
           xhttp.open("GET", "lock_set?state="+lock_sts, true);
           xhttp.send();
         }
         //setInterval(function() {
         //  getData();
         //}, 2000); 
      </script>
   </center>
   </body>
</html>
)=====";
