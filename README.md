## Utilizando os conceitos abordados em aula, crie um firmware que seja capaz de enviar a temperatura do núcleo do micro controlador via MQTT para um tópico a sua escolha. O envio deve ser feito sempre que o botão da placa Raspberry Pi Pico for pressionado.


# Como funciona?
O raspberry pico através do código em C faz a leitura da temperatura e ao pressionar o botão da placa envia os dados utilizando
MQTT para o python que irá receber a informação e printar no console

# Linguagem C 
Para compilar o projeto no Windows, execute os seguintes comandos no diretório raiz do projeto
```console
mkdir build
cd build
cmake ..
ninja
```

Para compilar o projeto em Sistemas Operacionais Baseados em Linux, execute os seguintes comandos no diretório raiz do projeto
```console
mkdir build
cd build
cmake ..
make
```

# Linguagem Python
Usar o comando pip para fazer instalação das bibliotecas necessárias

```console
!pip install paho
```


# Curiosidades
Para garantir maior segurança da parte do servidor (raspberry pi) e do lado do client (python) utilizei o conceito de ENV para 
armazenar configurações de endereço de broker, garantindo a segurança das informações