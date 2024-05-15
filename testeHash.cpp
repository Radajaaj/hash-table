#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::cin;

int string2int(string str);



//Vai ter uma classe hash, que vai armazenar:
    // * array de ponteiros para structs (conteudo da tabela hash)
    // * n° máx de indices do array acima
    // * n° de indices ja ocupados

//Vai ter uma struct do tipo de dado a ser armazenado.
//  Dá pra ser uma tabela que armazena produtos numa loja de conveniência (ComeTofuWell 2.0)
    // * string Nome do produto
    // * string Categoria
    // * int    ID do produto (random)
    // * float  Quantidade
    // * float  Preço

    //Daí ele pega o ID, concatena com o valor numérico da string do nome da categoria, e converte na posição da tabela hash

//E é isso! hora de implementar

//Primeiro, criamos a classe dos produtos:
class produto{
    private:
        string nome;
        string categoria;
        int id;
        float quantidade;
        float preco;

    public:
        produto(string nome, string categoria, float quantidade, float preco){
            this->nome       = nome;
            this->categoria  = categoria;
            this->quantidade = quantidade;
            this->preco      = preco;
            this->id         = rand();      //Limite definido pela constante RAND_MAX = 0x7fff (32767), segundo a documentação.
        }

        //GETTERS
        string getNome(){
            return nome;
        }
        string getCategoria(){
            return categoria;
        }
        int getID(){
            return id;
        }
        float getQuantidade(){
            return quantidade;
        }
        float getPreco(){
            return preco;
        }

        //SETTERS
        void setNome(string novoNome){
            nome = novoNome;
        }
        void setCategoria(string novoCategoria){
            categoria = novoCategoria;
        }
        void setID(int novoID){
            id = novoID;
        }
        void setQuantidade(float novoQuantidade){
            quantidade = novoQuantidade;
        }
        void setPreco(float novoPreco){
            preco = novoPreco;
        }

        //Funções para alterar o estoque
        float venderProduto(float quantiaVendida){ //Recebe a quantia a ser vendida. Retorna o preço a ser pago. Atualiza o estoque. Se retornar 0, estoque insuficiente.
            if(quantiaVendida > quantidade){    //Estoque insuficiente
                return 0;
            }

            setQuantidade(getQuantidade() - quantiaVendida);    //Atualiza o estoque

            return (preco * quantiaVendida);    //Retorna o valor da compra
        }

        bool adicionarEstoque(float estoqueExtra){
            if(estoqueExtra <= 0){ //Operação invalida
                return false;
            }
            setQuantidade(getQuantidade() + estoqueExtra);      //Adiciona produtos ao estoque
            return true;
        }


        int funcaoHashString(string nome, int MAX_SIZE){
            return (string2int(nome)) % MAX_SIZE;
        }


        void printMe(int MAX_SIZE){ //Printa os dados do produto
            cout << endl;
            cout << endl << ";-------- " << getNome() << " --------;";
            cout << endl << ";--- Categoria: " << getCategoria();
            cout << endl << ";--- Estoque: " << getQuantidade();
            cout << endl << ";--- Preco: " << getPreco();
            cout << endl << ";--- ID: " << getID();
            cout << endl << ";--- Posicao na tabela: " << funcaoHashString(getNome(), MAX_SIZE);
            cout << endl << ";-------- ---------- --------; " << endl;
        }
};


//Segundo, criamos a classe da tabela Hash:
class hash{
    private:
        int MAX_SIZE;           //Tamanho máximo da tabela
        int espacos_ocupados;   //Quantidade de espaços já ocupados na tabela
        produto **array;        //Array que vai armazenar ponteiros para as classes dos produtos (acho que podemos acessar os dados como se fosse uma matriz)

    public:

        hash(int tamanho){ //Método construtor. Recebe o tamanho da tabela hash, e aloca espaço na memória para o array. ATENÇÂO! Idealmente, tamanho deve ser primo e não ser potência de 2.
            this->MAX_SIZE = tamanho;
            this->espacos_ocupados = 0;

            //Aqui alocamos um espaço na memória pro array de ponteiros (daí nós só alocamos espaço na memória pra novos produtos quando for necessário):
            this->array = (struct produto**)malloc(tamanho * sizeof(produto*)); //Com certeza existe alguma forma mais fácil de fazer isso em C++.

            for(int i = 0; i < tamanho; i++){ //Inicializamos o vetor com NULL's
                this->array[i] = NULL;
            }
        }

        int getSize(){
            return this->MAX_SIZE;
        }
        int getOcupados(){
            return this->espacos_ocupados;
        }

        void reiniciaTabela(){   //Desaloca todos os dados armazenados na tabela
            this->espacos_ocupados = 0;

            for(int i = 0; i < this->MAX_SIZE; i++){
                if(this->array[i] != NULL){ //Se tiverem dados dentro do coiso
                    delete this->array[i];   //Usamos delete, pois ali armazenamos classes criadas com o operador new class
                } 
            }
            free(array);    //Usamos free(), pois dentro de array alocamos uma memória usando malloc()
            
            this->array = NULL;
            this->MAX_SIZE = 0;
            //Agora, se quisermos, podemos só fazer um delete hash, ou reutilizar a classe para outra coisa (chamando a função construtora de novo, eu imagino)  
        }


        int funcaoHash(produto *hashed){ //Recebe o ponteiro para uma classe produto, e calcula o índice dele na tabela hash.
                        //Calculamos um valor numérico a partir do nome do produto
                        //Daria para somar ao ID do produto, pra adicionar aleatoriedade ao hashing (e evita exploits) (hashing universal)
                            //Mas isso estraga a busca :(
                        //E obtemos o resto da divisão do valor pelo tamanho máximo da tabela hash.
            return (string2int(hashed->getNome())) % this->MAX_SIZE;
        }

        int funcaoHashString(string nome){
            return (string2int(nome)) % this->MAX_SIZE;
        }


        int insercaoSemColisao(produto *inserido){ //Sem tratamento de colisão

            if(this->espacos_ocupados == this->MAX_SIZE){ //Checamos se a tabela já está cheia
                return 0;
            }
            int posicao = funcaoHash(inserido);    //Calculamos a posição aonde o produto será inserido

            this->array[posicao] = inserido;       //Array é um vetor de endereços
            this->espacos_ocupados =+ 1;
            cout << endl << "ID = " << inserido->getID() << " | Posicao = " << posicao << endl;
            return 1;
        }

        int insercaoComColisao(produto *inserido){
            return inserido->getID();
        }


        void cadastraProduto(){
            int op = 0;
            string nome, categoria;
            float quantidade, preco;

            while(op != 1){     //Inserimos os dados do produto
                cout << endl << endl << "Insira o nome do produto\nR: ";
                cin >> nome;
                cout << endl << "Insira a categoria do produto\nR: ";
                cin >> categoria;
                cout << endl << "Insira a quantidade em estoque do produto\nR: ";
                cin >> quantidade;
                cout << endl << "Insira o preço do produto\nR: ";
                cin >> preco;

                cout << endl << endl << "Escreva 1 se estiver tudo certo:\nNome: " << nome << " | Categoria: " << categoria << " | Preco: " << preco << " |     Quantidade: " << quantidade << endl << "R: ";
                cin >> op;
            }
            op = 2;
            produto *novo_produto = new produto(nome, categoria, quantidade, preco);    //Criamos um espaço na memória para armazenar o produto

            //while(op != 0 && op != 1){
            //    cout << endl << endl << "Voce deseja inserir com ou sem colisao?\n0 - Sem colisao  |  1 - Com colisao\nR: ";
            //    cin >> op;
            //}
            //
            //if(op == 0){
                op = insercaoSemColisao(novo_produto);
            //}else{
            //    op = insercaoComColisao(novo_produto);
            //}

            if(op == 0){
                cout << endl << "Erro na inserção! Tabela provavelmente cheia." << endl;
            }

        }


        produto *buscaSemColisao(string nome){  //Retorna o valor armazenado no array
            return this->array[funcaoHashString(nome)];
        }

        produto *buscaComColisao(string nome){  //Nao implementado
            return this->array[funcaoHashString(nome)];
        }

        produto *buscaDados(string nome){
            //int op = 2;
            //while(op != 0 && op != 1){
            //    cout << endl << endl << "0 - Busca sem colisao  |  1 - Busca com colisao\nR: ";
            //    cin >> op;
            //}
            //if(op == 0){
                return buscaSemColisao(nome);
            //}else{
            //    return buscaComColisao(nome);
            //}
        }


        void mostrarProdutos(){
            cout << endl << endl << "Produtos cadastrados: " << endl;
            for(int i = 0; i < this->MAX_SIZE; i++){
                if(array[i] != NULL){
                    this->array[i]->printMe(this->MAX_SIZE);
                }
            }
        }

};


int string2int(string str){ //Recebe uma string, retorna seu equivalente numérico
    int valor = 7;

    for(long long unsigned i = 0; i < str.length(); i++){
        valor = 31 * valor + (int) str[i];
    }
    if(valor < 0){
        valor = -1 * valor;
    }
    return valor;
}


hash *novoHash(){
    int TAMANHO_MAX;
    cout << endl << endl << "Qual sera o tamanho maximo da hash table?\n(Recomendam-se numeros primos grandes. Sugestao: 101)\nR: ";
    cin >> TAMANHO_MAX;

    hash *tabela = new hash(TAMANHO_MAX);

    cout << endl << "Tabela criada com sucesso!!" << endl;
    return tabela;
}


int main(){
    int op = 2;

    while(op != 0 && op != 1){
        cout << endl << endl << "Deseja criar uma hash table?\n1 - Sim  |  0 - Nao\nR: ";
        cin >> op;
    }
    if(op == 0){
        cout << endl << endl << "ok";
        exit(0);
    }
    hash *hashTable = novoHash();

    cout << "amogus\n Tamanho da hash table: " << hashTable->getSize() << " | Espacos ocupados: " << hashTable->getOcupados() << endl;

    op = 2;
    string nome;
    produto *temp;
    float quantidade;
    while(op != 0){
        system("pause");
        system("cls");
        cout << endl << endl << "O que voce deseja fazer?\n1 - Inserir produto\n2 - Buscar nome cadastrado\n3 - Reiniciar a tabela\n4 - Mostrar todos os produtos cadastrados\n5 - Vender Produto\n6 - Adicionar Estoque\n0 - Sair\nR: ";
        cin >> op;
        switch(op){
            case 1:
                hashTable->cadastraProduto();
            break;
            case 2:
                cout << endl << "Qual nome voce quer ver se esta cadastrado?\nR: ";
                cin >> nome;
                temp = hashTable->buscaDados(nome);
                if(temp != NULL){
                    temp->printMe(hashTable->getSize());
                }
            break;
            case 3:
                hashTable->reiniciaTabela();
                delete hashTable;
                hashTable = novoHash();
            break;
            case 4:
                hashTable->mostrarProdutos();
            break;
            case 5:
                cout << endl << "Qual o nome do produto a ser vendido?\nR: ";
                cin >> nome;
                cout << endl << "Em que quantidade?\nR: ";
                cin >> quantidade;
                temp = hashTable->buscaDados(nome);
                if(temp != NULL){
                    quantidade = temp->venderProduto(quantidade);
                    if(quantidade == 0){
                        cout << endl << "Estoque insuficiente!";
                    }else{
                        cout << endl << "Preco a se pagar: R$ "<< quantidade << endl;
                    }
                }else{
                    cout << endl << "Produto nao encontrado!";
                }
            break;
            case 6:
                cout << endl << "Qual o nome do produto?\nR: ";
                cin >> nome;
                cout << endl << "Quanto de estoque sera adicionado?\nR: ";
                cin >> quantidade;
                temp = hashTable->buscaDados(nome);
                if(temp != NULL){
                    temp->adicionarEstoque(quantidade);
                }
            break;
            case 0:
                cout << endl << "Tchau!";
            break;
            default:
                cout << endl << "Argumento invalido!";
            break;
        }
    }


    return 0;
}