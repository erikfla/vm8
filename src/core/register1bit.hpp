class Register1bit {
private:
    DLatch latch;

public:
    void load(bool value, bool enable) {
        latch.update(value, enable);
    }

    bool value() const {
        return latch.output();
    }

    void reset() {
        latch.update(false, true);  // Sett Q til 0
    }
};
