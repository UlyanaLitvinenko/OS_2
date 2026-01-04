package org.example.dto;

import jakarta.validation.constraints.NotNull;
import org.example.model.TaskStatus;

public class TaskStatusPatch {
    @NotNull
    private TaskStatus status;

    public TaskStatus getStatus() { return status; }
    public void setStatus(TaskStatus status) { this.status = status; }
}
