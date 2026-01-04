package org.example.config;

import org.springframework.amqp.core.Queue;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class RabbitConfig {

    @Bean
    public Queue taskQueue() {
        return new Queue("tasks", true); // durable=true значит очередь сохраняется
    }
}
