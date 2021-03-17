
[![N|Solid](https://eventos.ifg.edu.br/semanai2c/wp-content/uploads/sites/7/2016/08/marca-ifg-2015-todas-as-verses.png)](https://www.ifg.edu.br/goiania)

# Multimedidor monofasico (ESP-32)
O objetivo deste trabalho é desenvolver um multimedidor de energia elétrica capaz de realizar a medição da corrente e da tensão em intervalos curtos o suficientes para mensurar o consumo em Kw/h bem como a potência instantânea da fase . Assim, projetou-se um protótipo que, através de sensores externos, captura amostras da corrente e da tensão e realiza o processamento de dados para a determinação do consumo e da potência instantânea, o protótipo projetado, além de apresentar os dado em um display, também envia os dados para uma plataforma nas nuvens para que os dados sejam salvos e uma linha histórica seja mantida.
### Video apresentaçoes do projeto:

<p align="center">
  <b>Video apresentação Carlos</b><br/>
  <a href="https://youtu.be/3okiUbaMF5Y"><img src="https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/MiniaturaCarlos.png">
</p>
 <br/><br/>
<p align="center">
  <b>Video apresentação Denner</b><br/>
  <a href="https://youtu.be/WYwgZHkGLjU"><img src="https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/MiniaturaDenner.png">
</p>
 <br/><br/>
<p align="center">
<b>Video apresentação Elivan</b><br/>
  <a href="https://youtu.be/p4I9QAHyrWM"><img src="https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/MiniaturaElivan.png">
</p>
 <br/><br/>
<p align="center">
<b>Video apresentação Lucas Rangel</b><br/>
  <a href="https://youtu.be/hnyyQ29T8nU"><img src="https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/MiniaturaLucas.png">
</p>

 <br/><br/>


#### Componentes:

  - ESP32 (38 Pinos)
  <br/><br/>
![](https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/ESP32-DEVKIT-V4-pinout-v2.jpg)
  <br/><br/>
  
  
  - Display OLED SSD1306
  <br/><br/>
![](https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/display7.jpg)
<br/><br/>

  - Sensor de tensão ZMTP101b
  <br/><br/>
![](https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/ZMPT101B.jpg)
<br/><br/>
  
  - Sensor de corrente ACS
  <br/><br/>
![](https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/Acs712.png)
<br/><br/>
  
  - Regulador de tensão
  <br/><br/>
![](https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/regtensao.jpg)
<br/><br/>
  
  - Buzzer piezoeletrico
  <br/><br/>
![](https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/buzzer_5v-300x300.jpg)
<br/><br/>
  
  - Push button
  <br/><br/>
![](https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/push.jpg)
<br/><br/>
  
  - Relê 3v
  <br/><br/>
![](https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/rele.jpg)
<br/><br/>

*Demais componentes discretos (Resistores, transistores e leds)

  
#### Montagem pratica:

  
  ![](https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/Circuito.jpg)
  

#### Configuração do software:
- Para gravar a firmware no microcontrolador utilizando a Arduino IDE  siga os passos descritos [aqui](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)

  - Logo após configurar o ESP32 na arduino IDE, clone o presente repositorio (É necessario ter um cliente [git](https://git-scm.com/) instalado ).Para clonar o diretorio execute no terminal ou no prompt:
	```sh
	$ git clone https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32.git
	```
  - Apos clonado abra o ProjetoIHM.ino, no menu de placas selecione a placa: "ESP32 Dev-Module".
	![](https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/selectPlaca.png)
	
  - Faça o upload do codigo para sua placa. Logo apos o upload o projeto já estara funcionando em seu microcontrolador
		OBS: Algumas placas ESP32 necessitam que o botao "boot" na placa, seja mantido pressionado durante o upload do codigo.


### Licença:


MIT
**Free Software, Hell Yeah!**
