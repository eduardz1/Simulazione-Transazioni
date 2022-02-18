data.frame(progetto_so)

boxplot(progetto_so$accuracy.conf0,
        progetto_so$accuracy.conf1,
        progetto_so$accuracy.conf3,
        horizontal = TRUE, outline = FALSE, names = c("conf0", "conf1", "conf3"))
boxplot(progetto_so$accuracy.conf2, horizontal = TRUE, names = "conf2")

plot(progetto_so$accuracy.conf3, col = "green")
points(progetto_so$accuracy.conf0, col = "red")
points(progetto_so$accuracy.conf1, col = "blue")

plot(progetto_so$accuracy.conf2, col = "orange")



summary(progetto_so$accuracy.conf0)
summary(progetto_so$accuracy.conf1)
summary(progetto_so$accuracy.conf2)
summary(progetto_so$accuracy.conf3)
sd(progetto_so$accuracy.conf0) # 5.070685
sd(progetto_so$accuracy.conf1) # 1.397974
sd(progetto_so$accuracy.conf2) # 1.357416
sd(progetto_so$accuracy.conf3) # 11.72579
moda(progetto_so$accuracy.conf3)
