# Script para coletar métricas das implementações

# Limpa os arquivos antes de começar o loop
> sequencial.txt
> multithread.txt
> sincronizado.txt

for i in {1..10}; do
  ./sequencial >> sequencial.txt
  ./multithread >> multithread.txt
  ./sincronizado >> sincronizado.txt
done

