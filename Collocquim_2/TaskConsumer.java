package org.example.service;

import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.stereotype.Service;

@Service
public class TaskConsumer {

    @RabbitListener(queues = "tasks")
    public void receiveTask(String message) {
        System.out.println("Получено сообщение: " + message);
    }
}
