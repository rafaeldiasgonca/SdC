#include <Servo.h>

//////////PLANTA//////////////////
Servo servoGate;      //Servo motor que abre e fechar o portão
int endKeyClose = 6;  //Chave fim de curso que indica portão fechado
int endKeyOpen = 7;   //Chave fim de curso que indica portão aberto
int trimpot = A0;     //Trimpote que monitora o percurso do portão


//ENTRADAS_DE_CONTROLE_PARA_ABRIR_E_FECHAR/////////////
int buttonClose = 8;  //Botão que fecha o portão
int buttonOpen = 9;   //Botão que abre o portão

///////////Variaveis////////////
float valClose = 0;  //Valor do trimpote que indica que o portão esta fechado
float valOpen = 0;   //Valor do trimpote que indica que o portão esta aberto
float valGate;       //Leitura do trimpote que monitora a posição do portão
int statusGate;      //Estado atual do portão -> 0=fechado, 1=aberto, e 2=qualquer psoição não fechado nem aberto
int newStatusGate;   //Estado desejado do portão 
int right = 70;      //Valor para fazer o motor girar e mover o portão para a direita
int left = 110;      //Valor para fazer o motor girar e mover o portão para a esquerda
int openGate = 1;    
int closeGate = 0;
int transicionGate = 2;

///////////// Janela de dados /////////////
int countReadValuesOpen = 0;
int countReadValuesClose = 0;
int lastFiveValClose[5];
int lastFiveValOpen[5];

void set_val_open(float openValue) {
  valOpen = openValue;
}

void set_val_close(float closeValue) {
  valClose = closeValue;
}

void check_data_window_open() {

  float openValue;
  
  if (valOpen == 0) {
    openValue = set_val(endKeyOpen, left);
    Serial.println("--------------------------");
    Serial.println("Primeira leitura para aberto: ");
    Serial.println(openValue);
    Serial.println("--------------------------");
    Serial.print("\n\n");
    set_val_open(openValue);
  } else {
    Serial.println("Quantidade de valores acumulados para aberto:");
    Serial.println(countReadValuesOpen);
    Serial.print("\n\n");
		openValue = analogRead(trimpot);
    if (countReadValuesOpen < 4) {
      lastFiveValOpen[countReadValuesOpen] = openValue;
      Serial.println("--------------------------");
      Serial.println("Valores foram armazenados para aberto: ");
      Serial.println(openValue);
      Serial.println("--------------------------");
      Serial.print("\n\n");
      countReadValuesOpen++;
    } else if (countReadValuesOpen == 4) {
      lastFiveValOpen[countReadValuesOpen] = openValue;
      Serial.println("--------------------------");
      Serial.println("Valores foram armazenados para aberto: ");
      Serial.println(openValue);
      Serial.println("--------------------------");
      Serial.print("\n\n");

      Serial.println("--------------------------------");
      Serial.println("5 valores foram lidos, tirar média\n\n");
      calc_new_ref_values_open();
      countReadValuesOpen = 0;
    }
  }
}

void calc_new_ref_values_open() {
  float sumOpenValues = valOpen;

  Serial.println("Antigos valores de referência para aberto: ");
  Serial.println(sumOpenValues);
  Serial.print("\n\n");

  for (int i = 0; i < 5; i++) {
    sumOpenValues += lastFiveValOpen[i];

    Serial.println("Somando com: ");
    Serial.println(lastFiveValOpen[i]);
    Serial.print("\n\n");
  }

  float newOpenValue = sumOpenValues / 6;
  set_val_open(newOpenValue);
  Serial.println("Novos valores de referência para aberto: ");
  Serial.println(newOpenValue);
  Serial.println("--------------------------------");
  Serial.print("\n\n");
}

void check_data_window_close() {
  float closeValue;

  if (valClose == 0) {
    closeValue = set_val(endKeyClose, right);
    Serial.println("--------------------------");
    Serial.println("Primeira leitura para fechado: ");
    Serial.println(closeValue);
    Serial.println("--------------------------");
    Serial.print("\n\n");
    set_val_close(closeValue);
  } else {
    Serial.println("Quantidade de valores acumulados para fechado:");
    Serial.println(countReadValuesClose);
    Serial.print("\n\n");
		closeValue = analogRead(trimpot);
    if (countReadValuesClose < 4) {
      lastFiveValClose[countReadValuesClose] = closeValue;
      Serial.println("--------------------------");
      Serial.println("Valores foram armazenados para fechado: ");
      Serial.println(closeValue);
      Serial.println("--------------------------");
      Serial.print("\n\n");
      countReadValuesClose++;
    } else if (countReadValuesClose == 4) {
      lastFiveValClose[countReadValuesClose] = closeValue;
      Serial.println("--------------------------");
      Serial.println("Valores foram armazenados para fechado: ");
      Serial.println(closeValue);
      Serial.println("--------------------------");
      Serial.print("\n\n");

      Serial.println("--------------------------------");
      Serial.println("5 valores foram lidos, tirar média\n\n");
      calc_new_ref_values_close();
      countReadValuesClose = 0;
    }
  }
}

void calc_new_ref_values_close() {
  float sumCloseValues = valClose;

  Serial.println("Antigos valores de referência para fechado: ");
  Serial.println(sumCloseValues);
  Serial.print("\n\n");

  for (int i = 0; i < 5; i++) {
    sumCloseValues += lastFiveValClose[i];

    Serial.println("Somando com: ");
    Serial.println(lastFiveValClose[i]);
    Serial.print("\n\n");
  }

  float newCloseValue = sumCloseValues / 6;
  set_val_close(newCloseValue);
  Serial.println("Novos valores de referência para fechado: ");
  Serial.println(newCloseValue);
  Serial.println("--------------------------------");
  Serial.print("\n\n");
}

float set_val(int endKey, int sentido) {
  float valReadTrimpot;
  //Serial.print("--------- Sentido: ");
  //Serial.println(sentido);
  while (digitalRead(endKey) != HIGH) {  //Fazer o motor ir para um sentido até achar a chave fim de curso
    servoGate.write(sentido);
  }
  servoGate.write(90);                   //fazer o motor parar
  valReadTrimpot = analogRead(trimpot);  //Registrar o valor lido no trimpote
  return valReadTrimpot;                 //Retornar o valor lido no trimpote
}

//Funções_para_abrir_e_fechar_o_portão
void move_gate(int newStatusGate) {
  Serial.print("--------- Status atual: ");
  Serial.println(statusGate);
  Serial.print("--------- Status para realizar: ");
  Serial.println(newStatusGate);
  if (statusGate != newStatusGate) {
    if (analogRead(trimpot) > valOpen) {
      Serial.println("--------- move_gate: ABRIR");
      open_gate();
    } else {
      Serial.println("--------- move_gate: FECHAR");
      close_gate();
    }
  }
}

void open_gate() {
  if(statusGate != openGate){
   	// Serial.println("\n\n------- Abrindo --------");
  	float trimpotCurrentValue = analogRead(trimpot);
  	//Serial.print("--------- Valor trimpot atual: ");
  	//Serial.println(trimpotCurrentValue);
  	//Serial.print("--------- Valor para aberto: ");
  	//Serial.println(valOpen);
  	while (trimpotCurrentValue >= valOpen) {
    	servoGate.write(110);
    	trimpotCurrentValue = analogRead(trimpot);
    	//Serial.print("--------- Valor trimpot atual: ");
    	//Serial.println(trimpotCurrentValue);
   	 	//Serial.print("--------- Valor para aberto: ");
    	//Serial.println(valOpen);
  	}
  	servoGate.write(90);
    check_data_window_open();
  	statusGate = openGate;
  }
}

void close_gate() {
  if(statusGate != closeGate){
    //Serial.println("\n\n------- Fechando --------");
  	float trimpotCurrentValue = analogRead(trimpot);
  	//Serial.print("--------- Valor trimpot atual: ");
  	//Serial.println(trimpotCurrentValue);
  	//Serial.print("--------- Valor para fechado: ");
  	//Serial.println(valClose);
  	while (trimpotCurrentValue <= valClose) {
    	servoGate.write(70);
   	 	trimpotCurrentValue = analogRead(trimpot);
    	//Serial.print("--------- Valor trimpot atual: ");
    	//Serial.println(trimpotCurrentValue);
    	//Serial.print("--------- Valor para fechado: ");
    	//Serial.println(valClose);
  	}
  	servoGate.write(90);
    check_data_window_close();
  	statusGate = closeGate;
  }
  
}

void comand() {
  if (digitalRead(buttonOpen) == HIGH) {
    //Serial.println("--------- Abrir pressionado ----------");
    open_gate();
  }
  if (digitalRead(buttonClose) == HIGH) {
    //Serial.println("--------- Fechar pressionado ----------");
    close_gate();
  }
}

void setup() {

  Serial.begin(9600);

  servoGate.attach(10);
  pinMode(endKeyClose, INPUT);
  pinMode(endKeyOpen, INPUT);
  pinMode(buttonClose, INPUT);
  pinMode(buttonOpen, INPUT);
  
  check_data_window_open();
  check_data_window_close();
}

void loop() {
  comand();
}
