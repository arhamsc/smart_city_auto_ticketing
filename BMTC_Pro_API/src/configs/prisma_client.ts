import { PrismaClient } from "@prisma/client";

let instance: SmartCityDBClass;
let prisma: PrismaClient;

class SmartCityDBClass {
  constructor() {
    if (instance) {
      throw new Error("New instance cannot be created.");
    }
    try {
      prisma = new PrismaClient();
      console.log("Prisma Available.");
    } catch (err) {
      console.log(err);
    }
    instance = this;
  }

  getPrismaClient(): PrismaClient {
    return prisma;
  }

  async disconnectPrisma() {
    await prisma.$disconnect();
  }
}

const SmartCityDB = Object.freeze(new SmartCityDBClass()); //Singleton Class

export default SmartCityDB;
