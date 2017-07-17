### Collectd 
get collectd latest from  https://github.com/aneeshkp/collectd

### edit collectd/collectd.conf

<Plugin connectivity>
  Interface eno2
</Plugin>
<Plugin write_kafka>
  Property "metadata.broker.list" "10.19.110.9:9092,10.19.110.11:9092"
  Property "queue.buffering.max.ms" "0"
  Property "batch.num.messages" "1"
  Property "retry.backoff.ms" "1"
  <Topic "interface1">
    Format JSON
  </Topic>
</Plugin>


### run collectd 
./collectd -C collectd.conf -f

# garuda
### Compiling agent
gcc -I ~/librdkafka/src -lrdkafka -lz -lpthread -lrt  garudaagent.c  -o garuda  -std=gnu99  -l yaml -lyajl

### start kafka listener 
./garuda interface1 -g -A891 -A -b 10.19.110.9:9092,10.19.110.11:9092

-g = consumer group 
interface1 topic
-b broker

