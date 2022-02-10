
[![N|Solid](https://eventos.ifg.edu.br/semanai2c/wp-content/uploads/sites/7/2016/08/marca-ifg-2015-todas-as-verses.png)](https://www.ifg.edu.br/goiania)

# Multimedidor monofasico - Instrumentação Industrial
O objetivo deste trabalho é desenvolver um multimedidor de energia elétrica capaz de realizar a medição da corrente e da tensão em intervalos curtos o suficientes para mensurar o consumo em Kw/h bem como a potência instantânea da fase . Assim, projetou-se um protótipo que, através de sensores analogicos, captura amostras dos sinais analogicos e realiza por meio de tecnicas de processamento digital de sinais obtem-se a corrente e a tensão, para então a determinação do consumo e da potência instantânea, o protótipo projetado, além de apresentar os dado em um display, também envia os dados para uma plataforma nas nuvens para que os dados sejam salvos e uma linha histórica seja mantida.
### Video apresentaçoes do projeto por integrante:

<div>
	<p align="center">
	  <b>Video apresentação Alisson Vieira</b><br/>
	  <a href="https://youtu.be/4bRhPEaAbvE"><img src="https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/AlissonVieira.png"></a>
	</p>
</div>
 <br/><br/>
 <div>
	<p align="center">
	  <b>Video apresentação Filipe Grzelak</b><br/>
	  <a href="https://youtu.be/YVH7wvP0Ib4"><img src="https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/FilipeGrzelak.png"></a>
	</p>
</div>
 <br/><br/>
<div>
	<p align="center">
	<b>Video apresentação Lucas Rangel</b><br/>
	  <a href="https://youtu.be/hXKg1XqKNyss"><img src="https://github.com/LucasRangelSSouza/MultimedidorMonofasicoESP32/blob/main/assets/lucasRangel.png"></a>
	</p>
</div>
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
