package org.example.controller;

import org.example.dto.TaskRequest;
import org.example.dto.TaskStatusPatch;
import org.example.model.Task;
import org.example.model.TaskStatus;
import org.example.service.TaskService;
import org.example.service.TaskProducer; // добавляем продюсер для RabbitMQ
import jakarta.validation.Valid;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/tasks")
public class TaskController {

    private final TaskService taskService;
    private final TaskProducer taskProducer; // внедряем продюсер

    public TaskController(TaskService taskService, TaskProducer taskProducer) {
        this.taskService = taskService;
        this.taskProducer = taskProducer;
    }

    @GetMapping
    public ResponseEntity<List<Task>> getAllTasks() {
        return ResponseEntity.ok(taskService.getAllTasks());
    }

    @GetMapping("/{id}")
    public ResponseEntity<Task> getTaskById(@PathVariable Long id) {
        return taskService.getTaskById(id)
                .map(ResponseEntity::ok)
                .orElseGet(() -> ResponseEntity.status(HttpStatus.NOT_FOUND).build());
    }

    @PostMapping
    public ResponseEntity<Task> createTask(@RequestBody @Valid TaskRequest request) {
        Task task = new Task(
                null,
                request.getTitle(),
                request.getDescription(),
                request.getStatus()
        );

        Task created = taskService.createTask(task);

        // Отправляем сообщение в RabbitMQ
        taskProducer.sendTask("Создана задача: " + created.getTitle());

        return ResponseEntity.status(HttpStatus.CREATED).body(created);
    }

    @PutMapping("/{id}")
    public ResponseEntity<Task> updateTask(
            @PathVariable Long id,
            @RequestBody @Valid TaskRequest request) {

        Task task = new Task(
                id, // при обновлении указываем id
                request.getTitle(),
                request.getDescription(),
                request.getStatus()
        );

        return taskService.updateTask(id, task)
                .map(updated -> {
                    taskProducer.sendTask("Обновлена задача: " + updated.getTitle());
                    return ResponseEntity.ok(updated);
                })
                .orElseGet(() -> ResponseEntity.status(HttpStatus.NOT_FOUND).build());
    }

    @PatchMapping("/{id}")
    public ResponseEntity<Task> patchTaskStatus(
            @PathVariable Long id,
            @RequestBody @Valid TaskStatusPatch patch) {

        TaskStatus status = patch.getStatus();

        return taskService.patchStatus(id, status)
                .map(updated -> {
                    taskProducer.sendTask("Изменён статус задачи: " + updated.getTitle() + " → " + updated.getStatus());
                    return ResponseEntity.ok(updated);
                })
                .orElseGet(() -> ResponseEntity.status(HttpStatus.NOT_FOUND).build());
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteTask(@PathVariable Long id) {
        boolean deleted = taskService.deleteTask(id);

        if (deleted) {
            taskProducer.sendTask("Удалена задача с id=" + id);
            return ResponseEntity.noContent().build();
        } else {
            return ResponseEntity.status(HttpStatus.NOT_FOUND).build();
        }
    }
}
