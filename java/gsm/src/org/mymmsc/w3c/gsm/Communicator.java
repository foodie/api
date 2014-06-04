/**
 * @(#)Communication.java	6.3.9 09/10/02
 *
 * Copyright 2000-2010 MyMMSC Software Foundation (MSF), Inc. All rights reserved.
 * MyMMSC PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
package org.mymmsc.w3c.gsm;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.TooManyListenersException;

import javax.comm.CommPortIdentifier;
import javax.comm.PortInUseException;
import javax.comm.SerialPort;
import javax.comm.SerialPortEvent;
import javax.comm.SerialPortEventListener;
import javax.comm.UnsupportedCommOperationException;

import org.mymmsc.w3c.dso.AbstractLibrary;
import org.mymmsc.w3c.gsm.pdu.PduContext;
import org.mymmsc.w3c.gsm.pdu.PduParser;

/**
 * ����ͨ��
 * 
 * @author WangFeng(wangfeng@yeah.net)
 * @version 6.3.9 09/10/02
 * @since mymmsc-gsm 6.3.9
 */
public class Communicator extends AbstractLibrary implements
		SerialPortEventListener {

	private Enumeration<?> m_port_list;
	private String m_port_name;
	private CommPortIdentifier m_port_id;// ���ڹ�����,�����,����,�ͳ�ʼ�����ڵȹ�����
	private SerialPort m_serial_port = null;
	private int m_reply_interval;
	private int m_command_delay;
	private String m_reply_string;
	private InputStream m_input_stream = null;
	private OutputStream m_output_stream = null;
	private int m_baud_rate;
	private String m_send_mode;
	private String m_message;
	// private int m_msgcount = 0;
	private boolean m_err_flag = false;
	private InBox m_inBox = null;
	private OutBox m_outBox = null;

	// private List<Integer> m_listsmg = new LinkedList<Integer>();

	public Communicator() {
		m_port_name = "COM1";
		m_baud_rate = 9600;
		m_send_mode = "0";
		m_reply_interval = 200;
		m_command_delay = 200;

		m_inBox = new InBox();
		m_outBox = new OutBox();
	}

	/**
	 * �г����д���
	 */
	public void listSerialPort() {
		// CommPortIdentifier���getPortIdentifiers���������ҵ�ϵͳ���еĴ��ڣ�
		// ÿ�����ڶ�Ӧһ��CommPortIdentifier���ʵ����
		m_port_list = null;
		m_port_id = null;
		m_port_list = CommPortIdentifier.getPortIdentifiers();
		info("�����б�");
		while (m_port_list.hasMoreElements()) {
			m_port_id = (CommPortIdentifier) m_port_list.nextElement();
			/* ����˿������Ǵ��ڣ����ӡ����˿���Ϣ */
			if (m_port_id.getPortType() == CommPortIdentifier.PORT_SERIAL) {
				info(m_port_id.getName());
			}
		}
		info("�����б���ʾ������");
	}

	/**
	 * ��ô���
	 */
	public void getSerialPort() {
		if (m_err_flag == true) {
			return;
		}
		info("����������...");
		if (m_port_name == "") {
			info("���ں�Ϊ�գ����������ļ���");
			m_err_flag = true;
			return;
			// System.out.println("Portname is null, get err, the program now exit!");
			// System.exit(0);
		}
		m_port_list = CommPortIdentifier.getPortIdentifiers();
		while (m_port_list.hasMoreElements()) {
			m_port_id = (CommPortIdentifier) m_port_list.nextElement();
			if ((m_port_id.getPortType() == CommPortIdentifier.PORT_SERIAL)
					&& m_port_id.getName().equalsIgnoreCase(m_port_name)) {
				try {
					m_serial_port = (SerialPort) m_port_id
							.open("SendSms", 2000);
				} catch (PortInUseException e) {
					info("��ȡ" + m_port_name + "ʱ����ԭ��" + e.getMessage());
					m_err_flag = true;
					return;
				}
			}
		}
	}

	public void listenSerialPort() {
		if (m_err_flag == true) {
			return;
		}
		if (m_serial_port == null) {
			info("������" + m_port_name + "������������ã�");
			m_err_flag = true;
			return;
		}
		// �������������
		try {
			m_output_stream = (OutputStream) m_serial_port.getOutputStream();
			m_input_stream = (InputStream) m_serial_port.getInputStream();
		} catch (IOException e) {
			info(e.getMessage());
		}
		try {
			// �����˿�
			m_serial_port.notifyOnDataAvailable(true);
			m_serial_port.notifyOnBreakInterrupt(true);
			m_serial_port.addEventListener(this);
		} catch (TooManyListenersException e) {
			m_serial_port.close();
			info(e.getMessage());
		}
		try {
			m_serial_port.enableReceiveTimeout(20);
		} catch (UnsupportedCommOperationException e) {
			//
		}
		// ���ö˿ڵĻ�������
		try {
			m_serial_port.setSerialPortParams(m_baud_rate,
					SerialPort.DATABITS_8, SerialPort.STOPBITS_1,
					SerialPort.PARITY_NONE);
		} catch (UnsupportedCommOperationException e) {
			e.printStackTrace();
		}
	}

	// �Դ��ڵĶ�д����
	public void writeToSerialPort(String msgString) {
		try {
			m_output_stream.write(msgString.getBytes());
			// CTRL+Z=(char)26
		} catch (IOException e) {
			System.out.println(e.getMessage());
		}
	}

	private void waitForRead(int waitTime) {
		try {
			Thread.sleep(waitTime);
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
	}

	public String readFromSerialPort(String message) {
		int strLength;
		String messageStr;
		String returnString = "";
		strLength = message.length();
		System.out.println("READ: " + message);
		if (strLength >= 6) {
			messageStr = message.substring(strLength - 4, strLength - 2);
			if (messageStr.equals("OK")) {
				returnString = messageStr;
			} else {
				returnString = message;
			}
			messageStr = message.substring(strLength - 6, strLength - 2);
			if (messageStr.equals("ERROR")) {
				returnString = messageStr;
			}
		}
		return returnString;
	}

	// �����������ر�������Դ
	public void closeSerialPort() {
		if (m_serial_port != null) {
			try {
				m_serial_port.close();
			} catch (RuntimeException e) {
				System.out.println(e.getMessage());
			}
		}
		info("�ѶϿ����ӣ�");
	}

	public void closeIOStream() {
		if (m_input_stream != null) {
			try {
				m_input_stream.close();
			} catch (IOException e) {
				System.out.println(e.getMessage());
			}
		}
		if (m_output_stream != null) {
			try {
				m_output_stream.close();
			} catch (IOException e1) {
				System.out.println(e1.getMessage());
			}
		}
		// returnStateInfo("�ѹر�I/O����");
	}

	public void setToNull() {
		if (m_serial_port != null) {
			m_serial_port.close();
		}
		m_serial_port = null;
		m_input_stream = null;
		m_output_stream = null;
	}

	/*
	 * �����¼� (non-Javadoc)
	 * 
	 * @see
	 * javax.comm.SerialPortEventListener#serialEvent(javax.comm.SerialPortEvent
	 * )
	 */
	public void serialEvent(SerialPortEvent e) {
		StringBuffer inputBuffer = new StringBuffer();
		int newData = 0;
		info("�¼�: " + CommEvent.getDesc(e.getEventType()));
		switch (e.getEventType()) {
		case SerialPortEvent.DATA_AVAILABLE:// DATA_AVAILABLE - �����ݵ���
			while (newData != -1) {
				try {
					newData = m_input_stream.read();
					if (newData == -1) {
						break;
					}
					// if ('\r' == (char) newData) {
					// inputBuffer.append('\n');
					// } else {
					inputBuffer.append((char) newData);
					// }
				} catch (IOException ex) {
					System.err.println(ex);
					return;
				}
			}
			info("������: " + inputBuffer.toString());
			m_message += new String(inputBuffer);
			if (m_message.trim().startsWith("+CMGL:")) {
				PduParser pp = new PduParser();
				PduContext pdu = null;
				String s = m_message.trim();
				String sline = "";
				int tmpIndex = -1;
				int tmpType = -1;
				int tmpLength = -1;

				for (int i = 0; i < s.length(); i++) {
					char ch = s.charAt(i);
					switch (ch) {
					case '+':
						sline += ch;
						break;
					case '\r':
					case '\n':
						sline = sline.trim();
						if (sline.length() > 1) {
							System.out
									.println("------------------------------>");
							System.out.println("<" + sline + ">");
							if (sline.charAt(0) == '+') {
								String tmpParams = null;
								// ����
								if (sline.startsWith(Constants.GSM_READ)) {
									// ���� ��Ϣ�б�: +CMGL: ����,����,,����<����>
									tmpParams = sline
											.substring(Constants.GSM_READ
													.length() + 1);
									String[] params = tmpParams.split(",");
									if (params.length == 4) {
										System.out.println("params[0]: "
												+ params[0]);
										tmpIndex = Integer.valueOf(
												params[0].trim()).intValue();
										tmpType = Integer.valueOf(
												params[1].trim()).intValue();
										tmpLength = Integer.valueOf(
												params[3].trim()).intValue();
									}
								}
							} else if (tmpIndex > 0) {
								// ������������
								pdu = pp.parsePdu(sline.trim());
								info(pdu.toString());
								if (pdu.getSmscAddress().length() > 5
										&& tmpIndex >= 0) {
									pdu.setIndex(tmpIndex);
									pdu.setInfoType(tmpType);
									pdu.setInfoLength(tmpLength);
									pdu.setInfoOps(0);
									// Ͷ�ݵ��ռ���
									m_inBox.add(pdu);
									tmpIndex = -1;
									tmpType = -1;
									tmpLength = -1;
								}
							}
							sline = "";
							System.out
									.println("------------------------------>");
						}
						break;
					default:
						sline += ch;
						break;
					}
				}
			}
			info("message: " + m_message);
			break;
		case SerialPortEvent.BI:// BI - ͨѶ�ж�.
			info("\n--- BREAK RECEIVED ---\n");
		}
	}

	/**
	 * ��Ϣ����
	 * 
	 * @param messageString
	 * @param phoneNumber
	 */
	public void sendmsg(String messageString, String phoneNumber) {
		// boolean sendSucc = false;
		getSerialPort();
		listenSerialPort();
		checkConn();
		if (m_err_flag == true) {
			return;
		}
		int msglength;
		String sendmessage, tempSendString;
		info("��ʼ����...");
		switch (Integer.parseInt(m_send_mode)) {
		case 0:// ��PDU��ʽ����
		{
			m_message = "";
			writeToSerialPort("AT+CMGF=0\r");
			waitForRead(m_command_delay);
			msglength = messageString.length();
			if (msglength < 8) {
				tempSendString = "000801" + "0"
						+ Integer.toHexString(msglength * 2).toUpperCase()
						+ asc2unicode(new StringBuffer(messageString));
			} else {
				tempSendString = "000801"
						+ Integer.toHexString(msglength * 2).toUpperCase()
						+ asc2unicode(new StringBuffer(messageString));
			}
			// "000801"˵������Ϊ00,08,01,
			// "00",��ͨGSM���ͣ��㵽�㷽ʽ
			// "08",UCS2����
			// "01",��Ч��
			if (phoneNumber.trim().length() > 0) {
				String[] infoReceiver = phoneNumber.split(",");
				int receiverCount = infoReceiver.length;
				if (receiverCount > 0) {
					for (int i = 0; i < receiverCount; i++) {
						sendmessage = "0011000D91" + "68"
								+ changePhoneNumber(infoReceiver[i])
								+ tempSendString;
						m_reply_string = readFromSerialPort(m_message);
						if (!m_reply_string.equals("ERROR")) {
							m_message = "";
							writeToSerialPort("AT+CMGS=" + (msglength * 2 + 15)
									+ "\r");
							waitForRead(m_command_delay);
							writeToSerialPort(sendmessage);
							try {
								m_output_stream.write((char) 26);
							} catch (IOException ioe) {
								//
							}
							getReply();
							if (m_reply_string.equals("OK")) {
								info("�ɹ����͵� " + infoReceiver[i]);
							}
							if (m_reply_string.equals("ERROR")) {
								System.out.println("���͸� " + infoReceiver[i]
										+ " ʱʧ�ܣ�");
							}
						}
					}
				}
			}
			break;
		}
		case 1:// ���ı���ʽ���ͣ����ܷ�������
		{
			m_message = "";
			writeToSerialPort("AT+CMGF=1\r");
			waitForRead(m_command_delay);
			if (phoneNumber.trim().length() > 0) {
				String[] infoReceiver = phoneNumber.split(",");
				int receiverCount = infoReceiver.length;
				if (receiverCount > 0) {
					for (int i = 0; i < receiverCount; i++) {
						m_reply_string = readFromSerialPort(m_message);
						if (!m_reply_string.equals("ERROR")) {
							writeToSerialPort("AT+CMGS=" + infoReceiver[i]
									+ "\r");
							waitForRead(m_command_delay);
							writeToSerialPort(messageString);
							try {
								m_output_stream.write((char) 26);
							} catch (IOException ioe) {
							}
							getReply();
							if (m_reply_string.equals("OK")) {
								info("�ɹ����͵� " + infoReceiver[i]);
							}
							if (m_reply_string.equals("ERROR")) {
								System.out.println("���͸� " + infoReceiver[i]
										+ " ʱʧ�ܣ�");
							}
						}
					}
				}
			}
			break;
		}
		default: {
			info("���ͷ�ʽ���ԣ����������ļ���");
			System.exit(0);
			break;
		}
		}
		closeIOStream();
		closeSerialPort();
		m_message = "";
		info("������ϣ�");
	}

	// ��ȡ���ж���
	public void readAllMessage(int readType) {
		getSerialPort();
		listenSerialPort();
		checkConn();
		if (m_err_flag == true) {
			return;
		}
		info("��ʼ��ȡ��Ϣ������ҪЩʱ�䣬��ȴ�...");
		// String tempAnalyseMessage = "";
		writeToSerialPort("AT+CMGF=0\r");
		waitForRead(m_command_delay);
		m_message = "";
		writeToSerialPort("AT+CMGL=" + readType + "\r");
		waitForRead(m_command_delay);
		try {
			getReply();
		} catch (Exception e) {
			//
		}
		info("��Ϣ��ȡ������");
		closeIOStream();
		closeSerialPort();
		m_message = "";
	}

	/**
	 * ��ȡָ������
	 * 
	 * @param msgIndex
	 */
	public void readMessage(int msgIndex) {
		getSerialPort();
		listenSerialPort();
		checkConn();
		if (m_err_flag == true) {
			return;
		}
		// String[] tempAnalyseMessage = null;
		writeToSerialPort("AT+CMGF=0\r");
		waitForRead(m_command_delay);
		m_message = "";
		if (msgIndex < 10) {
			writeToSerialPort("AT+CMGR=0" + msgIndex + "\r");
		} else {
			writeToSerialPort("AT+CMGR=" + msgIndex + "\r");
		}
		waitForRead(m_command_delay);
		try {
			getReply();
		} catch (Exception e) {
			//
		}
		closeIOStream();
		closeSerialPort();
		m_message = "";
	}

	/**
	 * �Զ���ʱ����д���
	 * 
	 * @param msgBuffer
	 * @return
	 */
	public String fixInfoTime(StringBuffer msgBuffer) {
		// msgBuffer.insert(12, "+");
		for (int i = 1; i < 3; i++) {
			msgBuffer.insert(12 - i * 2, ":");
		}
		msgBuffer.insert(6, " ");// ����������ʱ��֮������ַ���
		for (int i = 1; i < 3; i++) {
			msgBuffer.insert(6 - i * 2, "-");// �����ꡢ�¡���֮������ַ���
		}
		return (new String(msgBuffer));
	}

	// �����������ڴ��еı�ʾ��ÿ2λΪ1�飬ÿ��2�����ֽ�����
	// ���������Ϊ����������ĩβ��'F'�ճ�ż����Ȼ���ٽ��б任��
	// ��Ϊ�ڼ�����У���ʾ���ָߵ�λ˳�������ǵ�ϰ���෴.
	// �磺"8613851872468" --> "683158812764F8"
	public String changePhoneNumber(String phoneNumber) {
		int numberLength = phoneNumber.length();
		if (phoneNumber.length() % 2 != 0) {
			phoneNumber = phoneNumber + "F";
			numberLength += 1;
		}
		char newPhoneNumber[] = new char[numberLength];
		for (int i = 0; i < numberLength; i += 2) {
			newPhoneNumber[i] = phoneNumber.charAt(i + 1);
			newPhoneNumber[i + 1] = phoneNumber.charAt(i);
		}
		return (new String(newPhoneNumber));
	}

	/**
	 * ת��ΪUNICODE����
	 * 
	 * @param msgString
	 * @return
	 */
	public String asc2unicode(StringBuffer msgString) {
		StringBuffer msgReturn = new StringBuffer();
		int msgLength = msgString.length();
		if (msgLength > 0) {
			for (int i = 0; i < msgLength; i++) {
				new Integer((int) msgString.charAt(0)).toString();
				msgReturn.append(new StringBuffer());
				String msgCheck = new String(Integer
						.toHexString((int) msgString.charAt(i)));
				if (msgCheck.length() < 4) {
					msgCheck = "00" + msgCheck;
				}
				msgReturn.append(new StringBuffer(msgCheck));
			}
		}
		return (new String(msgReturn).toUpperCase());
	}

	/**
	 * UNICODE����ת��Ϊ��������
	 * 
	 * @param msgString
	 * @return
	 */
	public String unicode2asc(String msgString) {
		int msgLength = msgString.length();
		char msg[] = new char[msgLength / 4];
		for (int i = 0; i < msgLength / 4; i++) {
			// UNICODE����ת��ʮ������������ת��Ϊ��������
			msg[i] = (char) Integer.parseInt((msgString.substring(i * 4,
					4 * i + 4)), 16);
		}
		return (new String(msg));
	}

	/**
	 * ���϶�ȡ�����ź�, ���յ�OK�ź�ʱ, ֹͣ��ȡ, ��ִ������Ĳ���
	 */
	public void getReply() {
		m_reply_string = readFromSerialPort(m_message);
		while (m_reply_string != null) {
			// if (m_reply_string.equals("OK") ||
			// m_reply_string.equals("ERROR")) {
			if (m_reply_string.indexOf("OK") >= 0
					|| m_reply_string.indexOf("ERROR") >= 0) {
				return;
			}
			waitForRead(m_reply_interval);
			m_reply_string = readFromSerialPort(m_message);
		}
	}

	/**
	 * ���GSM Modem���������Ӵ���
	 */
	public void checkConn() {
		if (m_err_flag == true) {
			return;
		}
		m_message = "";
		writeToSerialPort("AT+CSCA?\r");
		waitForRead(m_command_delay);
		getReply();
		if (m_reply_string.equals("ERROR")) {
			info("Modem ���ֻ��������������飡");
			m_err_flag = true;
			closeIOStream();
			closeSerialPort();
			return;
		}
		info("����������");
	}

	/**
	 * ɾ������
	 * 
	 * @param msgIndex
	 */
	public void remove(int msgIndex) {
		m_message = "";
		getSerialPort();
		listenSerialPort();
		checkConn();
		writeToSerialPort("AT+CMGF=0\r");
		waitForRead(m_command_delay);
		getReply();
		if (m_reply_string.equals("OK")) {
			m_message = "";
			try {
				writeToSerialPort("AT+CMGD=" + msgIndex + "\r");
			} catch (RuntimeException e) {
				System.out.println(e.getMessage());
			}
		}
		closeIOStream();
		closeSerialPort();
	}

	/**
	 * �����Ϣ����
	 * 
	 * @param box
	 */
	private void removeBox(AbstractBox box) {
		int idx = -1;
		for (PduContext pdu : box) {
			idx = pdu.getIndex();
			info("����ɾ����" + idx + "������.");
			remove(idx);
		}
		// box.clear();
	}

	/**
	 * ɾ��������Ϣ
	 */
	public void removeAll() {
		removeBox(m_inBox);
		removeBox(m_outBox);
	}

	public void sendAll() {
		int idx = -1;
		int tmpSize = m_inBox.size();
		for (int i = 0; i < tmpSize; i++) {
			PduContext pdu = m_inBox.get(0);
			idx = pdu.getIndex();
			sendmsg(pdu.getDecodedText(), pdu.getAddress().substring(2));
			info("����ɾ����" + idx + "������.");
			remove(idx);
			m_inBox.remove(0);
		}
		// m_inBox.clear();
	}

	public void checkAll() {
		int idx = -1;
		int tmpSize = m_inBox.size();
		for (int i = 0; i < tmpSize; i++) {
			PduContext pdu = m_inBox.get(0);
			idx = pdu.getIndex();
			if(pdu.getAddress().indexOf("10658658") >=0) {
				info("****************************************************************");
				info(pdu.getAddress() + " >> " + pdu.getDecodedText());
				info("****************************************************************");
			}
			remove(idx);
			m_inBox.remove(0);
		}
		// m_inBox.clear();
	}

	private void info(String errInfo) {
		System.out.println(errInfo);
		logger.info(errInfo);
	}

	@Override
	public boolean close() {
		// TODO Auto-generated method stub
		closeIOStream();
		closeSerialPort();
		return true;
	}

	@Override
	public String getUnixClass() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getWindowsClass() {
		// TODO Auto-generated method stub
		return null;
	}
}
