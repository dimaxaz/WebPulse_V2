#include "KafkaProducer.hpp"
#include <iostream>

KafkaProducer::KafkaProducer(const std::string& brokers, const std::string& topic) 
    : topic_(topic) {
    
    std::string errstr;
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    
    conf->set("bootstrap.servers", brokers, errstr);
    conf->set("queue.buffering.max.messages", "100000", errstr);
    conf->set("queue.buffering.max.ms", "50", errstr);
    conf->set("batch.num.messages", "10000", errstr);

    producer_.reset(RdKafka::Producer::create(conf, errstr));
    if (!producer_) {
        throw std::runtime_error("Failed to create producer: " + errstr);
    }

    RdKafka::Conf* tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    topic_ptr_.reset(RdKafka::Topic::create(producer_.get(), topic_, tconf, errstr));
    if (!topic_ptr_) {
        throw std::runtime_error("Failed to create topic: " + errstr);
    }

    delete conf;
    delete tconf;
}

KafkaProducer::~KafkaProducer() {
    flush();
}

bool KafkaProducer::produce(const std::string& message) {
    RdKafka::ErrorCode err = producer_->produce(
        topic_ptr_.get(),
        RdKafka::Topic::PARTITION_UA,
        RdKafka::Producer::RK_MSG_COPY,
        const_cast<char*>(message.c_str()),
        message.size(),
        nullptr, nullptr
    );

    if (err != RdKafka::ERR_NO_ERROR) {
        std::cerr << "Failed to produce message: " 
                  << RdKafka::err2str(err) << std::endl;
        return false;
    }

    producer_->poll(0);
    return true;
}

void KafkaProducer::flush(int timeout_ms) {
    producer_->flush(timeout_ms);
} 