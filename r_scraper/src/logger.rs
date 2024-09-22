#[macro_export]
macro_rules! info {
    ($($arg:tt)*) => ({
        use colored::Colorize;
        println!("{} {}", "[INFO]".green(), format!($($arg)*));
    });
}

#[macro_export]
macro_rules! error {
    ($($arg:tt)*) => ({
        use colored::Colorize;
        println!("{} {}", "[ERROR]".red(), format!($($arg)*));
    });
}
