package org.example.service;

import org.example.model.Task;
import org.example.model.TaskStatus;
import org.example.repository.TaskRepository;
import org.springframework.cache.annotation.CacheEvict;
import org.springframework.cache.annotation.Cacheable;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class TaskService {

    private final TaskRepository taskRepository;

    public TaskService(TaskRepository taskRepository) {
        this.taskRepository = taskRepository;
    }

    @Cacheable("tasks_all")
    public List<Task> getAllTasks() {
        return taskRepository.findAll();
    }

    @Cacheable(value = "task_by_id", key = "#id")
    public Optional<Task> getTaskById(Long id) {
        return taskRepository.findById(id);
    }

    @CacheEvict(value = {"tasks_all", "task_by_id"}, allEntries = true)
    public Task createTask(Task task) {
        return taskRepository.save(task);
    }

    @CacheEvict(value = {"tasks_all", "task_by_id"}, allEntries = true)
    public Optional<Task> updateTask(Long id, Task task) {
        return taskRepository.findById(id).map(existing -> {
            existing.setTitle(task.getTitle());
            existing.setDescription(task.getDescription());
            existing.setStatus(task.getStatus());
            return taskRepository.save(existing);
        });
    }

    @CacheEvict(value = {"tasks_all", "task_by_id"}, allEntries = true)
    public Optional<Task> patchStatus(Long id, TaskStatus status) {
        return taskRepository.findById(id).map(t -> {
            t.setStatus(status);
            return taskRepository.save(t);
        });
    }

    @CacheEvict(value = {"tasks_all", "task_by_id"}, allEntries = true)
    public boolean deleteTask(Long id) {
        if (!taskRepository.existsById(id)) {
            return false;
        }
        taskRepository.deleteById(id);
        return true;
    }
}
